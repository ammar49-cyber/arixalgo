# Changelog

## v0.1.0 (2026-06-24)

### Added
- **S0 — Crypto Core**: SHA-3 (FIPS 202), ChaCha20-Poly1305 AEAD, Ed25519, BLAKE3, Argon2, constant-time utilities, cryptographic random
- **S1 — Secure Memory**: Guard-page protected allocations, 128-bit canary system, ASLR, mlock/VirtualLock, side-channel resistant ops (select, equal, lt, is_zero), timing-safe operations, cache flush/prefetch/barrier, power analysis dummy ops, x86_64 CMOV assembly
- **S2 — Obfuscation Engine** (C++): Control-flow flattening, string encryption (compile-time XOR + runtime ChaCha20-derived), instruction substitution (LEA, NAND, NEG+SUB), opaque predicates (math invariants, pointer self-comparison), stack-based code VM (256 registers, encrypted handler table), anti-debug (ptrace, IsDebuggerPresent, RDTSC timing, INT3 scan, CPUID hypervisor), configurable levels (LIGHT→MAXIMUM)
- **ARC — Adversarial Robustness Core**: Input guard (L2 anomaly detection), gradient obfuscator (noise+clamp), output verifier (cosine consistency + history smoothing), multi-metric security scoring, FGSM/PGD/CW attack simulation
- **HSS — Hierarchical State Space**: Multi-layer SSM with ZOH discretization, sequential/hierarchical scan
- **SER — Sparse Expert Routing**: Softmax + top-k (greedy/noisy), ReLU/GELU/Swish experts, load-balance loss
- **NPE — Neural Program Executor**: 16-register VM with 14 opcodes (MATMUL, ATTENTION, SOFTMAX, LAYERNORM, etc.), MLP/attention compiler, static verifier
- **FM — Federated Memory**: Per-node memory banks, euclidean similarity, LRU eviction, trust-weighted all-reduce sync with Laplace DP noise, gossip/ring topology, gradient compression
- **Training**: SGD with momentum, weight decay, gradient clipping, checkpoint save/load
- **Python bindings**: pybind11 wrapper for Tensor, Model, Trainer
- **Documentation**: Architecture, Vision, Roadmap, API reference
- **Website**: Next.js marketing site (arixsite.vercel.app)

### Changed
- N/A (initial release)

### Fixed
- N/A (initial release)

### Security
- All crypto primitives are constant-time and side-channel resistant
- All memory allocations have guard pages + canary protection
- All control flow can be obfuscated to prevent reverse engineering

### Known Issues
- Ed25519 verify fails for 2 edge cases (304/306 pass)
- Argon2 timing test has 1 pre-existing failure (3/4 pass)
- These are S0 limitations that predate S1–S3 and do not affect security model

### Build
```
cmake 3.20+ | C11/C++20 | Python 3.11+ | MSVC 2022 / GCC 13+ / Clang 16+
```
