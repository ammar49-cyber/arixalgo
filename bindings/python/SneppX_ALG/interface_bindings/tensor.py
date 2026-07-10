"""Tensor Module — Hybrid CPU/CUDA tensor with optional C backend and NumPy fallback."""

from typing import List, Optional, Tuple, Union, Callable
import ctypes
import math
import os
import numpy as np

# ---- Try to load the C (pybind11) backend ----
_C_BACKEND = None
try:
    from .. import _arix_c as _C_BACKEND
except ImportError:
    try:
        from .. import _SNEPPX_c as _C_BACKEND
    except ImportError:
        pass

if _C_BACKEND is not None:
    try:
        Dtype = _C_BACKEND.SNEPPXDtype
        Device = _C_BACKEND.SNEPPXDevice
        Layout = _C_BACKEND.SNEPPXLayout
        _HAS_C_BACKEND = True
    except AttributeError:
        _C_BACKEND = None
        _HAS_C_BACKEND = False
else:
    _HAS_C_BACKEND = False

if not _HAS_C_BACKEND:
    class Dtype:
        FLOAT32 = 0; FLOAT64 = 1; FLOAT16 = 2; BFLOAT16 = 3
        INT32 = 4; INT64 = 5; INT16 = 6; INT8 = 7; UINT8 = 8; BOOL = 9
    class Device:
        CPU = 0; CUDA = 1; METAL = 2; VULKAN = 3
    class Layout:
        DENSE = 0; CSR = 1; COO = 2

DTYPE_MAP = {
    "float32": (ctypes.c_float, 4, np.float32),
    "float16": (ctypes.c_uint16, 2, np.float16),
    "int32": (ctypes.c_int32, 4, np.int32),
    "int64": (ctypes.c_int64, 8, np.int64),
    "uint8": (ctypes.c_uint8, 1, np.uint8),
}

_NP_TO_DTYPE = {}
for k, (_, _, np_dt) in DTYPE_MAP.items():
    _NP_TO_DTYPE[np.dtype(np_dt)] = k


def _resolve_dtype(dtype) -> str:
    if dtype is None:
        return "float32"
    if isinstance(dtype, str):
        return dtype
    if isinstance(dtype, np.dtype):
        return _NP_TO_DTYPE.get(dtype, "float32")
    if hasattr(dtype, 'name'):
        return dtype.name.lower()
    return "float32"


def _numpy_dtype(dtype) -> np.dtype:
    return DTYPE_MAP.get(_resolve_dtype(dtype), (None, None, np.float32))[2]


class _CTensorData:
    def __init__(self, shape, dtype="float32"):
        self.shape = tuple(shape)
        self.dtype = _resolve_dtype(dtype)
        self.size = math.prod(shape) if shape else 0
        _, itemsize, _ = DTYPE_MAP.get(self.dtype, (None, 4, np.float32))
        self.nbytes = self.size * itemsize
        self._cptr = None
        self._is_cuda = False
        if self.size > 0:
            self._data = bytearray(self.nbytes)

    def _copy_from(self, arr: np.ndarray):
        if self.size > 0:
            self._data[:] = arr.tobytes()

    def to_numpy(self):
        _, _, np_dt = DTYPE_MAP.get(self.dtype, (None, 4, np.float32))
        return np.frombuffer(self._data, dtype=np_dt).reshape(self.shape).copy()


class Tensor:
    def __init__(self, data=None, shape=None, dtype="float32", device="cpu", requires_grad=False):
        self.device = device
        self.dtype = _resolve_dtype(dtype)
        self.requires_grad = requires_grad
        self.grad = None
        self._grad_fn = None
        if isinstance(data, (int, float, np.integer, np.floating)):
            shape = shape or (1,)
            self._data = _CTensorData(shape, self.dtype)
            arr = np.full(shape, float(data), dtype=_numpy_dtype(self.dtype))
            self._data._copy_from(arr)
            self.shape = shape
        elif isinstance(data, (list, tuple)):
            arr = np.array(data, dtype=_numpy_dtype(self.dtype))
            self._data = _CTensorData(arr.shape, self.dtype)
            self._data._copy_from(arr)
            self.shape = arr.shape
        elif isinstance(data, np.ndarray):
            dt = _resolve_dtype(dtype) or _resolve_dtype(str(data.dtype))
            self._data = _CTensorData(data.shape, dt)
            self._data._copy_from(data.astype(_numpy_dtype(dt)))
            self.shape = tuple(data.shape)
        elif isinstance(data, Tensor):
            self._data = data._data
            self.shape = data.shape
        else:
            shape = shape or (1,)
            self._data = _CTensorData(shape, self.dtype)
            self.shape = shape

    @property
    def dtype_name(self) -> str:
        return self.dtype

    @property
    def shape(self):
        return self._data.shape

    @shape.setter
    def shape(self, s):
        self._data.shape = tuple(s)

    @property
    def ndim(self):
        return len(self.shape)

    @property
    def numel(self):
        return self._data.size

    @property
    def data(self) -> np.ndarray:
        return self._data.to_numpy()

    @data.setter
    def data(self, arr: np.ndarray):
        self._data = _CTensorData(arr.shape, _resolve_dtype(str(arr.dtype)))
        self._data._copy_from(arr)
        self.shape = arr.shape

    @property
    def T(self):
        if self.ndim < 2:
            return self
        return self.transpose(0, 1)

    def numpy(self) -> np.ndarray:
        return self._data.to_numpy()

    def to(self, device: str):
        self.device = device
        return self

    def cuda(self):
        return self.to("cuda")

    def cpu(self):
        return self.to("cpu")

    def item(self) -> float:
        return float(self.data.flat[0])

    def view(self, *shape):
        t = Tensor(self)
        t.shape = shape
        return t

    def reshape(self, *shape):
        return self.view(*shape)

    def clone(self):
        t = Tensor(self)
        t._data = _CTensorData(self.shape, self.dtype)
        t._data._data[:] = self._data._data[:]
        return t

    def copy_(self, src: "Tensor"):
        if self.shape == src.shape:
            self._data._data[:] = src._data._data[:]
        return self

    def fill_(self, value):
        arr = np.full(self.shape, value, dtype=_numpy_dtype(self.dtype))
        self._data._copy_from(arr)
        return self

    def zero_(self):
        self._data._data = bytearray(self._data.nbytes)
        return self

    def _apply_unary(self, fn: Callable):
        return Tensor(fn(self.data), dtype=self.dtype)

    def _apply_binary(self, other, fn: Callable):
        a = self.data
        b = other.data if isinstance(other, Tensor) else other
        return Tensor(fn(a, b), dtype=self.dtype)

    def __add__(self, other):
        if isinstance(other, (int, float)):
            return Tensor(self.data + other, dtype=self.dtype)
        return self._apply_binary(other, lambda a, b: a + b)

    def __radd__(self, other):
        return self.__add__(other)

    def __sub__(self, other):
        if isinstance(other, (int, float)):
            return Tensor(self.data - other, dtype=self.dtype)
        return self._apply_binary(other, lambda a, b: a - b)

    def __rsub__(self, other):
        if isinstance(other, (int, float)):
            return Tensor(other - self.data, dtype=self.dtype)
        return Tensor(other.data - self.data, dtype=self.dtype)

    def __mul__(self, other):
        if isinstance(other, (int, float)):
            return Tensor(self.data * other, dtype=self.dtype)
        return self._apply_binary(other, lambda a, b: a * b)

    def __rmul__(self, other):
        return self.__mul__(other)

    def __truediv__(self, other):
        if isinstance(other, (int, float)):
            return Tensor(self.data / other, dtype=self.dtype)
        return self._apply_binary(other, lambda a, b: a / b)

    def __rtruediv__(self, other):
        if isinstance(other, (int, float)):
            return Tensor(other / self.data, dtype=self.dtype)
        return Tensor(other.data / self.data, dtype=self.dtype)

    def __matmul__(self, other):
        a, b = self.data, other.data if isinstance(other, Tensor) else other
        return Tensor(a @ b, dtype=self.dtype)

    def __pow__(self, other):
        if isinstance(other, (int, float)):
            return Tensor(self.data ** other, dtype=self.dtype)
        return self._apply_binary(other, lambda a, b: a ** b)

    def __neg__(self):
        return self._apply_unary(lambda a: -a)

    def __getitem__(self, key):
        return Tensor(self.data[key], dtype=self.dtype)

    def __setitem__(self, key, value):
        arr = self.data
        if isinstance(value, Tensor):
            v = value.item() if value.numel == 1 else value.data
        else:
            v = value
        arr[key] = v
        self._data._copy_from(arr)

    def __repr__(self):
        return f"Tensor(shape={self.shape}, dtype={self.dtype}, device={self.device})"

    def __len__(self):
        return self.shape[0] if self.shape else 0

    def __iter__(self):
        for i in range(len(self)):
            yield self[i]

    def mean(self, dim=None):
        if dim is None:
            return Tensor(float(self.data.mean()), dtype=self.dtype)
        return Tensor(self.data.mean(axis=dim), dtype=self.dtype)

    def sum(self, dim=None):
        if dim is None:
            return Tensor(float(self.data.sum()), dtype=self.dtype)
        return Tensor(self.data.sum(axis=dim), dtype=self.dtype)

    def var(self, dim=None):
        if dim is None:
            return Tensor(float(self.data.var()), dtype=self.dtype)
        return Tensor(self.data.var(axis=dim), dtype=self.dtype)

    def std(self, dim=None):
        if dim is None:
            return Tensor(float(self.data.std()), dtype=self.dtype)
        return Tensor(self.data.std(axis=dim), dtype=self.dtype)

    def min(self):
        return float(self.data.min())

    def max(self):
        return float(self.data.max())

    def sqrt(self):
        return self._apply_unary(lambda a: np.sqrt(a))

    def exp(self):
        return self._apply_unary(lambda a: np.exp(a))

    def log(self):
        return self._apply_unary(lambda a: np.log(a + 1e-10))

    def abs(self):
        return self._apply_unary(lambda a: np.abs(a))

    def relu(self):
        return self._apply_unary(lambda a: np.maximum(0, a))

    def sigmoid(self):
        return self._apply_unary(lambda a: 1.0 / (1.0 + np.exp(-a)))

    def tanh(self):
        return self._apply_unary(lambda a: np.tanh(a))

    def tanh_act(self):
        return self._apply_unary(lambda a: np.tanh(a))

    def gelu(self):
        return self._apply_unary(lambda a: 0.5 * a * (1.0 + np.tanh(0.79788456 * (a + 0.044715 * a**3))))

    def silu(self):
        return self._apply_unary(lambda a: a * (1.0 / (1.0 + np.exp(-a))))

    def softmax(self, dim=-1):
        a = self.data
        e = np.exp(a - a.max(axis=dim, keepdims=True))
        return Tensor(e / e.sum(axis=dim, keepdims=True), dtype=self.dtype)

    def log_softmax(self, dim=-1):
        a = self.data
        e = np.exp(a - a.max(axis=dim, keepdims=True))
        sm = e / e.sum(axis=dim, keepdims=True)
        return Tensor(np.log(sm + 1e-10), dtype=self.dtype)

    def transpose(self, dim1=0, dim2=1):
        return Tensor(self.data.swapaxes(dim1, dim2), dtype=self.dtype)

    def squeeze(self, dim=None):
        if dim is None:
            return Tensor(np.squeeze(self.data), dtype=self.dtype)
        return Tensor(np.squeeze(self.data, axis=dim), dtype=self.dtype)

    def unsqueeze(self, dim):
        return Tensor(np.expand_dims(self.data, dim), dtype=self.dtype)

    def expand(self, *shape):
        return Tensor(np.broadcast_to(self.data, shape), dtype=self.dtype)

    def backward(self, grad_output=None):
        if not self.requires_grad:
            return
        if grad_output is None:
            grad_output = Tensor(np.ones_like(self.data), dtype=self.dtype)
        self.grad = grad_output

    def detach(self):
        t = Tensor(self)
        t.requires_grad = False
        return t

    def save(self, path: str):
        np.save(path, self.data)

    @staticmethod
    def load(path: str):
        arr = np.load(path)
        return Tensor(arr)

    def mse_loss(self, target):
        diff = self.data - target.data
        return Tensor(np.array([np.mean(diff ** 2)]), dtype="float32")

    def cross_entropy(self, target):
        sm = np.exp(self.data - self.data.max(axis=-1, keepdims=True))
        sm = sm / sm.sum(axis=-1, keepdims=True)
        loss = -np.mean(target.data * np.log(sm + 1e-10))
        return Tensor(np.array([loss]), dtype="float32")

    def mae_loss(self, target):
        return Tensor(np.array([np.mean(np.abs(self.data - target.data))]), dtype="float32")

    def nll_loss(self, target):
        return Tensor(np.array([-np.mean(self.data * target.data)]), dtype="float32")

    def kl_div(self, target):
        return Tensor(np.array([np.mean(target.data * (np.log(target.data + 1e-10) - self.data))]), dtype="float32")

    def binary_cross_entropy(self, target):
        p = np.clip(self.data, 1e-10, 1 - 1e-10)
        loss = -np.mean(target.data * np.log(p) + (1 - target.data) * np.log(1 - p))
        return Tensor(np.array([loss]), dtype="float32")

    def conv1d(self, kernel, stride=1, padding=0):
        from scipy import signal
        arr = self.data
        k = kernel.data if isinstance(kernel, Tensor) else kernel
        if padding > 0:
            arr = np.pad(arr, [(0,0), (padding,), (0,)] if arr.ndim == 3 else [(padding,)])
        out = signal.correlate(arr, k, mode='valid')[..., ::stride]
        return Tensor(out, dtype=self.dtype)

    def conv2d(self, kernel, stride_h=1, stride_w=1, pad_h=0, pad_w=0):
        from scipy import signal
        arr = self.data
        k = kernel.data if isinstance(kernel, Tensor) else kernel
        if pad_h > 0 or pad_w > 0:
            arr = np.pad(arr, [(0,0), (pad_h, pad_h), (pad_w, pad_w), (0,)] if arr.ndim == 4 else [(pad_h, pad_h), (pad_w, pad_w)])
        out = signal.correlate(arr, k, mode='valid')
        out = out[..., ::stride_h, ::stride_w]
        return Tensor(out, dtype=self.dtype)

    def pool1d(self, kernel_size, stride=None):
        stride = stride or kernel_size
        arr = self.data
        out = np.array([arr[..., i:i+kernel_size].mean(axis=-1) for i in range(0, arr.shape[-1] - kernel_size + 1, stride)])
        if arr.ndim == 2:
            out = out.T
        return Tensor(out, dtype=self.dtype)

    def pool2d(self, kernel_h, kernel_w, stride_h=None, stride_w=None):
        stride_h = stride_h or kernel_h
        stride_w = stride_w or kernel_w
        arr = self.data
        out = np.array([[
            arr[..., i:i+kernel_h, j:j+kernel_w].mean(axis=(-2, -1))
            for j in range(0, arr.shape[-1] - kernel_w + 1, stride_w)]
            for i in range(0, arr.shape[-2] - kernel_h + 1, stride_h)])
        out = out.transpose(2, 3, 0, 1) if arr.ndim == 4 else out
        return Tensor(out, dtype=self.dtype)

    def dropout(self, rate=0.5, seed=42):
        rng = np.random.RandomState(seed)
        mask = rng.binomial(1, 1.0 - rate, self.shape).astype(np.float32)
        mask /= (1.0 - rate)
        return Tensor(self.data * mask, dtype=self.dtype)

    def layer_norm(self, gamma, beta, eps=1e-5):
        g = gamma.data if isinstance(gamma, Tensor) else gamma
        b = beta.data if isinstance(beta, Tensor) else beta
        arr = self.data
        mean = arr.mean(axis=-1, keepdims=True)
        var = arr.var(axis=-1, keepdims=True)
        return Tensor((arr - mean) / np.sqrt(var + eps) * g + b, dtype=self.dtype)

    def batch_norm(self, gamma, beta, running_mean, running_var, eps=1e-5):
        g = gamma.data if isinstance(gamma, Tensor) else gamma
        b = beta.data if isinstance(beta, Tensor) else beta
        rm = running_mean.data if isinstance(running_mean, Tensor) else running_mean
        rv = running_var.data if isinstance(running_var, Tensor) else running_var
        return Tensor(g * (self.data - rm) / np.sqrt(rv + eps) + b, dtype=self.dtype)

    def group_norm(self, gamma, beta, num_groups, eps=1e-5):
        g = gamma.data if isinstance(gamma, Tensor) else gamma
        b = beta.data if isinstance(beta, Tensor) else beta
        arr = self.data
        N, C, H, W = arr.shape
        arr_g = arr.reshape(N, num_groups, C // num_groups, H, W)
        mean = arr_g.mean(axis=(2, 3, 4), keepdims=True)
        var = arr_g.var(axis=(2, 3, 4), keepdims=True)
        arr_n = (arr_g - mean) / np.sqrt(var + eps)
        return Tensor(arr_n.reshape(N, C, H, W) * g.reshape(1, C, 1, 1) + b.reshape(1, C, 1, 1), dtype=self.dtype)

    def embedding(self, indices):
        idx = indices.data.astype(np.int64) if isinstance(indices, Tensor) else np.array(indices, dtype=np.int64)
        return Tensor(self.data[idx], dtype=self.dtype)

    @staticmethod
    def cat(tensors: List["Tensor"], dim=0) -> "Tensor":
        return Tensor(np.concatenate([t.data for t in tensors], axis=dim))

    @staticmethod
    def stack(tensors: List["Tensor"], dim=0) -> "Tensor":
        return Tensor(np.stack([t.data for t in tensors], axis=dim))

    @staticmethod
    def concat(tensors: List["Tensor"], dim=0) -> "Tensor":
        return Tensor.cat(tensors, dim)

    # Factory methods
    @staticmethod
    def zeros(shape, dtype="float32", device="cpu"):
        return Tensor(np.zeros(shape, dtype=_numpy_dtype(dtype)), dtype=dtype, device=device)

    @staticmethod
    def ones(shape, dtype="float32", device="cpu"):
        return Tensor(np.ones(shape, dtype=_numpy_dtype(dtype)), dtype=dtype, device=device)

    @staticmethod
    def randn(shape, dtype="float32", device="cpu"):
        return Tensor(np.random.randn(*shape).astype(_numpy_dtype(dtype)), dtype=dtype, device=device)

    @staticmethod
    def rand(shape, dtype="float32", device="cpu"):
        return Tensor(np.random.rand(*shape).astype(_numpy_dtype(dtype)), dtype=dtype, device=device)

    @staticmethod
    def arange(start, stop=None, step=1, dtype="float32"):
        return Tensor(np.arange(start, stop, step).astype(_numpy_dtype(dtype)), dtype=dtype)

    @staticmethod
    def eye(n, dtype="float32"):
        return Tensor(np.eye(n).astype(_numpy_dtype(dtype)), dtype=dtype)

    @staticmethod
    def full(shape, fill_value, dtype="float32"):
        return Tensor(np.full(shape, fill_value, dtype=_numpy_dtype(dtype)), dtype=dtype)

    @staticmethod
    def from_numpy(arr: np.ndarray, dtype=None):
        dt = _resolve_dtype(dtype) or _resolve_dtype(str(arr.dtype))
        return Tensor(arr, dtype=dt)


def cat(tensors: List[Tensor], dim=0) -> Tensor:
    return Tensor.cat(tensors, dim)


def stack(tensors: List[Tensor], dim=0) -> Tensor:
    return Tensor.stack(tensors, dim)


Tensorable = Union[Tensor, np.ndarray]

__all__ = ["Tensor", "Tensorable", "Dtype", "Device", "Layout", "_HAS_C_BACKEND", "_C_BACKEND", "cat", "stack"]
