# SNEPPX-ALG

**Next-generation cognitive processing system with security built into the foundation** — v0.9.5.748

[![CI](https://github.com/ammar49-cyber/sneppx-alg/actions/workflows/ci.yml/badge.svg)](https://github.com/ammar49-cyber/sneppx-alg/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Python 3.9+](https://img.shields.io/badge/python-3.9+-blue.svg)](https://www.python.org/downloads/)
[![C11](https://img.shields.io/badge/C-11-555555.svg)](https://en.wikipedia.org/wiki/C11_(C_standard_revision))
[![Lines](https://img.shields.io/badge/183K%20lines-891%20files-blue)](https://github.com/ammar49-cyber/sneppx-alg)

---

## Overview

**SNEPPX-ALG** is a next-generation cognitive processing system with neural architecture search, hierarchical state spaces, mixture of experts, and a full S0–S9 security layer. Written in C11 + C++20 + MASM, with complete Python bindings.

### Key Features

- **Tensor Engine**: SIMD-optimized (AVX2/AVX-512), CUDA kernels (Ampere/Hopper), 100+ autodiff ops
- **Neural Architectures**: HSS (Mamba/S6), SER (MoE), ARC (Adversarial), NPE (Neural Program Engine), FM (Fractal Memory), Differential Attention, FlexAttention
- **LLM Zoo**: from_pretrained() for LLaMA 2/3, Mistral, Qwen2, DeepSeek V2 with weight converters
- **Distributed Training**: ZeRO-1/2/3, Pipeline/Tensor/Expert Parallelism, Elastic, NCCL, RDMA
- **Security-First**: 10 security layers (S0-S9) — post-quantum crypto, code obfuscation, AI safety, formal verification
- **Quantization**: INT8/INT4/FP8/AWQ/GPTQ with CUDA kernels and Python API
- **Hardware Drivers**: CUDA, ROCm (dynamic loading), TPU (PjRt-style), NPU, Vulkan, Metal, oneAPI, Intel, AMD, Qualcomm QNN, SGX
- **Network Protocols**: TCP sockets, RDMA, gRPC, HTTP server, WebSocket, MQTT, QUIC, DNS service discovery, Raft consensus
- **File Formats**: SafeTensors, ONNX, PyTorch, HDF5, NumPy, custom checkpoint
- **Storage**: S3, GCS, Azure Blob
- **Python Bindings**: ~160 Python modules across all subsystems

---

## Quick Start

### From PyPI

```bash
pip install sneppx-alg
pip install sneppx-alg[serve]   # inference server
pip install sneppx-alg[hf]      # HuggingFace integration
pip install sneppx-alg[dev]     # dev dependencies
```

### From Source

```bash
git clone https://github.com/ammar49-cyber/sneppx-alg.git
cd sneppx-alg
pip install -e .
```

### C/C++ Build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release

# Specific targets
cmake --build build --config Release --target neural_core_kernel
cmake --build build --config Release --target neural_security_c
cmake --build build --config Release --target neural_architecture_layer
```

### Quick Python Example

```python
from SneppX_ALG import Tensor, Linear, AdamW, Trainer, TrainConfig
import numpy as np

model = Linear(768, 10)
config = TrainConfig(learning_rate=3e-4, batch_size=32, max_steps=1000, optimizer="adamw")
trainer = Trainer(model, config)

x = Tensor.from_numpy(np.random.randn(32, 768).astype(np.float32))
y = Tensor.from_numpy(np.random.randn(32, 10).astype(np.float32))
trainer.fit([(x, y)])
```

---

## Architecture

```
SNEPPX-ALG (v0.9.5.748)
├── Core Kernel (C11 / CUDA)
│   ├── tensor/          SIMD GEMM, tensor ops, expressions, strided copy, broadcast
│   ├── autodiff/        Reverse-mode autodiff (100+ ops, tape, checkpointing)
│   ├── optimizer/       SGD, Adam/W, Lion, LAMB, LARS, AdaFactor, RAdam, SF-AdamW
│   ├── train/           Trainer with gradient clipping, AMP, LR schedulers
│   ├── cuda/            Flash Attention v2/v3, GEMM, LayerNorm, Softmax, fused AdamW
│   ├── attention/       Multi-head, Differential, Flex, Flash, Mamba2
│   ├── arch/            Mamba-2, gated activations, YaRN, ALiBi
│   ├── thread/          Work-stealing thread pool + workload dispatch
│   ├── memory/          Pool allocator (lock-free Treiber stack), TLS cache
│   ├── data/            Data pipeline, CSV parser, text dataset with tokenizer
│   ├── distributed/     ZeRO 1-3, DDP, pipeline/TP/EP, elastic, heartbeat, NCCL
│   ├── quantization/    INT8/FP8/AWQ/GPTQ quant/dequant
│   ├── inference/       Inference engine
│   ├── model_zoo/       LLM config presets + weight converters
│   ├── profiler.c       NVTX markers, kernel timing, JSON export
│   └── logger.c         Structured JSON logging, ANSI colors, per-rank
├── Algorithms
│   ├── hss/             Hierarchical State Space (Mamba/S6, S4, selective scan)
│   ├── ser/             Sparse Expert Routing (MoE, top-k gating, load balancing)
│   ├── arc/             Adversarial Robustness Certification (PGD, FGSM, smoothing)
│   ├── npe/             Neural Programming Engine (compiler, VM, JIT)
│   ├── fm/              Fractal Memory (node, bank, sync, distributed controller)
│   ├── gcn/             Graph Convolutional Networks (skeleton)
│   ├── rl/              Reinforcement Learning (skeleton)
│   ├── gan/             Generative Adversarial Networks (skeleton)
│   ├── diffusion/       Diffusion Models (skeleton)
│   ├── rnn/             Recurrent Neural Networks (skeleton)
│   ├── transformer/     General Transformer (skeleton)
│   └── vit/             Vision Transformers (skeleton)
├── Security (S0-S9)
│   ├── crypto/          Post-quantum: Kyber, Dilithium, SPHINCS+, Falcon + classical
│   ├── memory/          S1: Guard pages, canaries, ASLR, secure allocator
│   ├── obfuscation/     S2: CFG flattening, inst sub, opaque preds, string/VMs
│   ├── monitor/         S3: Integrity, container breakout, ML anomaly detection
│   ├── network/         S4: TLS 1.3, Noise, DDoS, identity, firewall
│   ├── ai/              S5/S6: RLHF, prompt/output filters, watermarking, explainability
│   ├── updates/         S7: TUF signed updates, bsdiff deltas, A/B partitions
│   ├── formal/          S8: TLA+ parser, LTL model checking, Lean 4 export
│   ├── pentest/         S9: CVE scanner, fuzzer, red team, compliance checker
│   └── ...              S10-S15: Hardware security, hardening, network, AI, quantum, identity
├── Networking
│   ├── socket/          TCP (WinSock2/POSIX) with message framing
│   ├── rdma/            Memory registration, QP lifecycle, read/write
│   ├── grpc/            Server/stub lifecycle, barrier, all-gather, tensor transfer
│   ├── http/            HTTP server with routing, middleware, static files (skeleton)
│   ├── websocket/       WebSocket client (skeleton)
│   ├── mqtt/            MQTT pub/sub client (skeleton)
│   ├── quic/            QUIC transport with streams (skeleton)
│   ├── dns/             Service discovery via DNS-SD (skeleton)
│   ├── monitoring/      Health check endpoints (skeleton)
│   └── consensus/       Raft consensus (skeleton)
├── Drivers
│   ├── cuda/            CUDA driver (dynamic loading, memcpy, kernel launch)
│   ├── rocm/            HIP dynamic loading + CPU fallback
│   ├── tpu/             PjRt-style client + host memory
│   ├── npu/             Neural Processing Unit (skeleton)
│   ├── sgx/             Intel SGX enclave (skeleton)
│   ├── oneapi/          Intel oneAPI / SYCL (skeleton)
│   ├── vulkan/          Vulkan compute pipeline (skeleton)
│   ├── metal/           Apple Metal compute (skeleton)
│   ├── qualcomm/        Qualcomm QNN (skeleton)
│   ├── amd/             AMD extended (skeleton)
│   ├── intel/           Intel native (skeleton)
│   └── shim/            Device abstraction layer (skeleton)
├── Memory Management
│   ├── vmem.c           VirtualAlloc/mmap reserve/commit/decommit
│   ├── compress.c       BFP4/BFP8 block floating-point, sparse bitmap
│   ├── slab_alloc.c     Slab allocator with local cache, GC
│   ├── pool/            Object pool (skeleton)
│   ├── arena/           Arena allocator (skeleton)
│   ├── huge/            Huge page manager (skeleton)
│   └── tiered/          Tiered memory (RAM + PMem + SSD) (skeleton)
├── File Formats & Storage
│   ├── format/          Checkpoint reader, SafeTensors, ONNX, PyTorch, HDF5, NumPy
│   └── storage/         S3, GCS, Azure Blob (skeletons)
└── Tooling
    ├── model_converter/ Format conversion (skeleton)
    ├── data_explorer/   Interactive data inspection (skeleton)
    ├── security_auditor/ Compliance checker (skeleton)
    └── config_generator/ Config file generator (skeleton)
```

---

## Project Structure

```
sneppx-alg/
├── kernel/              Core computational substrate (C11)
├── algorithms/          Neural architecture algorithms
├── security/            S0-S9 security layers (C, C++, MASM, Rust, Go, C#)
├── net/                 Networking layer
├── drivers/             Hardware drivers
├── mm/                  Memory management
├── fs/                  File system & formats
├── include/             Public C/C++ headers
├── bindings/            Language bindings (Python, C++, Rust)
├── tests/               Test suites (C unit, Python, security, fuzz, chaos, benchmark)
├── examples/            Demo programs
├── scripts/             Build & CI scripts
├── tools/               CLI tools
├── config/              Configuration files
├── docs/                Documentation
├── benchmarks/          Benchmark programs
├── lib/                 Support libraries (hashtable, pqueue, rbtree, strutil)
└── cmake/               CMake modules
```

---

## Supported Architectures

| Category | Models |
|----------|--------|
| **LLMs** | LLaMA 2/3 (7B/13B/70B), Mistral 7B, Qwen2 (7B/72B), DeepSeek V2 (Lite/Full) |
| **Custom** | HSS (Mamba/S6), SER (MoE), ARC (Adversarial), NPE (Neural Program Engine), FM (Fractal Memory) |
| **Planned** | GCN, RL, GAN, Diffusion, RNN, Transformer, ViT |

---

## Security

**10 layers (S0-S9) + extended (S10-S15):**

| Layer | Description | Status |
|-------|-------------|--------|
| S0 | Build Integrity (SBOM, Reproducible Builds, TUF) | Complete |
| S1 | Memory Hardening (Guard Pages, Canaries, ASLR, Locked Memory) | Complete |
| S2 | Code Obfuscation (CFG Flattening, Inst Sub, Opaque Preds, String/VM) | Complete |
| S3 | Runtime Monitoring (Integrity, Container Breakout, ML Anomaly) | Complete |
| S4 | Network Security (TLS 1.3, Noise, mTLS, DDoS, Port Knocking) | Complete |
| S5 | AI Safety (RLHF, Differential Privacy, Prompt/Output Filters) | Complete |
| S6 | AI Sanitizer (Semantic Injection, Encoded Attack Detection) | Complete |
| S7 | Supply Chain (TUF Signed Updates, A/B Partitions, Canary) | Complete |
| S8 | Formal Verification (TLA+, LTL Model Checking, Lean 4 Export) | Complete |
| S9 | Penetration Testing (CVE Scanner, Fuzzer, Compliance) | Complete |
| S10-15 | Hardware, Hardening, Network, AI, Quantum, Identity | Planned |

**Post-Quantum Crypto**: Kyber (ML-KEM), Dilithium (ML-DSA), SPHINCS+, Falcon
**Classical Crypto**: AES-GCM, ChaCha20-Poly1305, Ed25519, X25519, SHA-3, BLAKE3, Argon2

---

## Testing

```bash
# Python tests (all pass, pure-Python fallback)
$env:PYTHONPATH = "bindings/python"
python -m pytest tests/python/ -v

# C tests (requires cmake build)
cmake --build build_dir --config Release
cd build_dir && ctest -C Release --output-on-failure
```

---

## Contributing

See [AGENTS.md](AGENTS.md) for development guidelines. Key points:
- C11 + C++20, MASM for x86-64 hot paths
- `SNEPPX_` prefix for all public symbols
- 4-space indentation, no tabs
- No VLAs (MSVC C11 limitation)

---

## License

MIT License — see [LICENSE](LICENSE) for details.

---

## Citation

```bibtex
@software{sneppx-alg,
  title = {SNEPPX-ALG: Secure Neural Architecture v0.9.5.748},
  author = {Ammar [SNEPPX]},
  year = {2026},
  url = {https://github.com/ammar49-cyber/sneppx-alg}
}
```

---

## Links

- **GitHub**: https://github.com/ammar49-cyber/sneppx-alg
- **PyPI**: https://pypi.org/project/sneppx-alg/
- **CI**: https://github.com/ammar49-cyber/sneppx-alg/actions
- **Releases**: https://github.com/ammar49-cyber/sneppx-alg/releases
- **Issues**: https://github.com/ammar49-cyber/sneppx-alg/issues
- **License**: https://github.com/ammar49-cyber/sneppx-alg/blob/main/LICENSE

---

*Release v0.9.5.748 — 183K+ lines, 891 source files, ~160 Python modules, 31+ test suites*
