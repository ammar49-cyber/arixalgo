"""ARIX-Algo — Python bindings for the cognitive processing system.

Usage:
    from arix_algo import _arix_c as ax
    t = ax._Tensor.zeros([4, 8], ax.FLOAT32)
    ax.crypto.sha3_256(b"data")
"""

from arix_algo._arix_c import *

__all__ = []
