# Python API Reference

## Status: ⚠️ Partial (v0.1.0)

Python bindings are built via Pybind11 (71 KB `bindings.cpp`) when `SNEPPX_BUILD_PYTHON=ON`.
Ready for forward-pass usage; backward/training depends on autodiff implementation.

## Installation

```bash
pip install -e src/python
```

Build with Python bindings:
```bash
cmake -B build -DSNEPPX_BUILD_PYTHON=ON
cmake --build build
```

## Quick Start

```python
import SneppX_ALG as ax

# Create a tensor
t = ax.Tensor.randn((4, 8, 16), dtype=ax.float32)
```

## Components (v0.9.7.890e)

| Component | Module | Status |
|-----------|--------|--------|
| Tensor | `ax.Tensor` `ax.tensor.*` | ✅ Creation, ops, reductions, IO, NN |
| Autodiff | `ax.Variable` `ax.Tape` | ✅ Forward + backward (real autodiff tape; layer-norm gamma/beta gradient fixed in v0.9.7.890e) |
| Optimizer | `ax.SGD` `ax.Adam` | ✅ SGD / Adam step |
| HSS | `ax.HSSModel` | ✅ Forward + training (`test_train_integration` converges deterministically) |
| SER | `ax.SERModel` | ✅ Forward pass |
| ARC | `ax.ARCModel` | ✅ Forward pass |
| NPE | `ax.NPEModel` | ✅ Compile + execute |
| FM | `ax.FMModel` | ✅ Read/write/sync |
| Trainer | `ax.Trainer` | ✅ Training loop (tape backward + optimizer step) |
| Security | `ax.s0`–`ax.s9` | ✅ S0–S9 complete |

## Environment Setup

```powershell
python -m venv .venv
.venv\Scripts\Activate.ps1
pip install -e src/python
```

The `.pyd` extension module is copied automatically by CMake when `SNEPPX_BUILD_PYTHON=ON`.
