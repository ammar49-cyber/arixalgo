# SneppX-ALG Documentation

## Overview

SneppX-ALG is a composable 5-component AI algorithm pipeline wrapped in 4 security layers. This is the first open-source AI algorithm with cryptographic integrity built into its foundation.

## Quickstart

### Build from source

```bash
git clone https://github.com/ammar49-cyber/sneppx-alg.git
cd sneppx-alg
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DSNEPPX_BUILD_TESTS=ON
cmake --build . -j$(nproc)
ctest --output-on-failure
```

### Run a demo

```bash
# After building:
./examples/hss_demo
./examples/ser_demo
./examples/arc_demo
./examples/npe_demo
./examples/fm_demo
```

### Run benchmarks

```bash
cmake .. -DSNEPPX_BUILD_BENCHMARKS=ON
cmake --build . -j$(nproc)
./tests/benchmark/bench_tensor
./tests/benchmark/bench_autodiff
```

## Architecture

```
                     ┌──────────────────────────────────────┐
                     │           Security Layer              │
                     │  S0 Crypto · S1 Secure Mem · S2 Obf  │
                      │  S3 Behavioral Monitor · S4-S9       │
                     └──────────────────────────────────────┘
                                     │
                     ┌───────────────▼──────────────────────┐
                     │         Algorithm Pipeline            │
                     │  HSS → SER → ARC → NPE → FM          │
                     │  (SSM) (MoE) (Guard) (VM)  (Fed Mem) │
                     └──────────────────────────────────────┘
                                     │
                      ┌───────────────▼──────────────────────┐
                      │         Integrity Layer               │
                      │  ZK Proofs (opt-in) · Formal Safety ·  │
                      │  On-Device Attestation                │
                      └──────────────────────────────────────┘
```

## Component Descriptions

### HSS — Hierarchical State Space

Multi-layer state space model with zero-order hold discretization. Processes sequences timestep-by-timestep (a parallel scan over the state dimension is planned for the CUDA path). Supports hierarchical decomposition for long-range dependencies. Training through the autodiff tape is supported (see `docs/hss_training.md`).

- `docs/architecture.md` for mathematical details
- `src/arch/src/hss/` for implementation
- `tests/unit/hss/` for tests

### SER — Sparse Expert Routing

Softmax-based routing with top-k selection. Each input token is routed to k experts, and the outputs are combined via weighted sum. Includes load-balancing loss to prevent expert collapse.

- `docs/architecture.md` for routing details
- `src/arch/src/ser/` for implementation
- `tests/unit/ser/` for tests

### ARC — Adversarial Robustness Core

Three-layer defense: input guard (z-score anomaly detection), gradient obfuscation (noise + clamping), output verifier (cosine similarity + temporal smoothing). Includes attack simulation (FGSM, PGD, CW).

- `docs/architecture.md` for threat model
- `src/arch/src/arc/` for implementation
- `tests/unit/arc/` for tests

### NPE — Neural Program Executor

16-register virtual machine with 14 opcodes (MATMUL, ATTENTION, SOFTMAX, LAYERNORM, etc.). Supports attention and MLP program compilation from network configurations. Includes static verifier for program correctness.

- `docs/architecture.md` for instruction set
- `src/arch/src/npe/` for implementation
- `tests/unit/npe/` for tests

### FM — Federated Memory

Per-node memory banks with euclidean similarity search and LRU eviction. Supports trust-weighted all-reduce synchronization with Laplace differential privacy noise. Gradient compression via random sampling.

- `docs/architecture.md` for sync protocols
- `src/arch/src/fm/` for implementation
- `tests/unit/fm/` for tests

## Status Table

| Component | Lines | Tests | Status |
|-----------|-------|--------|
| Tensor Core | ~2,000 | 57+27 edge | ✅ Real |
| Memory | ~800 | 13 | ✅ Real |
| Thread Pool | ~300 | 11 | ✅ Real |
| HSS | ~500 | 2 + integration | ✅ Real (training works) |
| SER | ~600 | 5 | ✅ Real |
| ARC | ~600 | 5 | ✅ Real |
| NPE | ~700 | 4 | ✅ Real |
| FM | ~600 | 4 | ✅ Real |
| Autodiff | ~400 | tape backward | ✅ Real (layer-norm fixed in v0.9.7.890e) |
| Optimizer | ~300 | 1 | ✅ Real (Adam/SGD) |
| Python API | ~500 | 3 | ✅ Real |
| S0 Crypto | ~2,000 | 10 | ✅ Real |
| S1 Secure Mem | ~800 | 3 | ✅ Real |
| S2 Obfuscation | ~1,500 | 4 | ✅ Complete |
| S3 Monitor | ~100 | 0 | ✅ Complete |

## Directory Structure

```
SneppX_ALG/
├── CMakeLists.txt
├── CMakePresets.json
├── src/
│   ├── core/                    # Foundation: tensor, memory, thread, autodiff, optimizer
│   ├── arch/                    # Algorithm pipeline: HSS, SER, ARC, NPE, FM, train
│   ├── security/
│   │   ├── c/                   # S0 — Crypto Core + S1 — Secure Memory (C)
│   │   ├── cpp/                 # S2 — Obfuscation Engine (C++)
│   │   ├── asm/                 # x86_64 assembly helpers
│   │   └── rust/                # Future: Rust security layer
│   └── python/                  # pybind11 bindings
├── tests/
│   ├── unit/                    # Component unit tests
│   ├── integration/             # Multi-component integration tests
│   ├── benchmark/               # Performance benchmarks
│   ├── security/                # S0 + S1 (C) tests
│   └── security/cpp/            # S2 (C++) tests
├── examples/                    # Demos for each component
├── docs/                        # Documentation
├── scripts/                     # Build and release scripts
└── cmake/                       # CMake modules
```

## Key Metrics

- **C/C++ source**: ~15,000 lines across all components
- **Tests**: 50 registered (47 pass, 2 pre-existing security edge cases, 1 slow thread test)
- **Build time**: ~30s on modern hardware (Release, 8 cores)
- **Dependencies**: None for C core. pybind11 for Python bindings (optional)

## Next Steps

- Read [docs/architecture.md](architecture.md) for deep technical details
- Read [docs/hss_training.md](hss_training.md) for an end-to-end HSS training walkthrough
- Read [docs/release_notes/v0.9.7.890e.md](release_notes/v0.9.7.890e.md) for the v0.9.7.890e release notes
- Read [docs/migration/v0.9.7.890e.md](migration/v0.9.7.890e.md) for the upgrade guide
- Read [docs/roadmap.md](roadmap.md) for the project timeline
- Read [docs/installation.md](installation.md) for platform-specific build guides
- Read [docs/api/c.md](api/c.md) for the C API reference
- Read [CONTRIBUTING.md](../CONTRIBUTING.md) to learn how to contribute
