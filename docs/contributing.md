# Contributing Guide

## Overview

SNEPPX-Algo accepts contributions via email patches. See [CONTRIBUTING.md](../CONTRIBUTING.md) for the full contributor agreement.

## Development Workflow

### Setup

```bash
git clone https://github.com/ammar49-cyber/sneppx-alg.git
cd sneppx-alg
cmake --preset debug
cmake --build build --config Debug -j$(nproc)
cd build && ctest --output-on-failure
```

### Code Organization

| Directory | Content |
|-----------|---------|
| `include/neural_core/` | Public headers |
| `kernel/` | Core implementations (tensor, memory, autodiff, optimizer, trainer) |
| `algorithms/` | Algorithm pipeline (HSS, SER, ARC, NPE, FM) |
| `security/` | S0-S9 security layer (crypto, memory, obfuscation, monitor, network, AI sanitizer, key vault, updates, formal, pentest) |
| `bindings/python/` | Python wrappers (pure Python, no pybind11 required) |
| `tests/` | C and Python test suites |
| `examples/` | Demos and examples |

### Testing

```powershell
# C tests
cd build && ctest -C Release --output-on-failure

# Python tests
$env:PYTHONPATH = "bindings/python"
python tests/python/test_algo_wrappers.py
```

## Contribution Areas

### Beginner Friendly

- Add new tensor operations and fused kernels
- Improve test coverage for edge cases
- Fix compiler warnings on non-MSVC platforms
- Add documentation for existing functions
- Optimize memory allocation paths

### Intermediate

- Extend the autodiff backward pass with new ops
- Add Python bindings for tensor operations
- Extend CUDA kernels (new GEMM/attention variants)
- Add GPU detection and fallback logic

### Advanced

- NPE JIT optimization passes (loop unrolling, reordering)
- NCCL distributed training with multi-node support
- Flash Attention v3 TMA+WGMMA full implementation
- Quantization-aware training integration
- Zero-knowledge proof backend for NPE execution

## Communication

- **Patches**: algoSNEPPX@gmail.com
- **Security**: algoSNEPPX@gmail.com

## Code Review

Patches are reviewed by the project maintainer. Expect feedback within 7 days.
