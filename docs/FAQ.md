# FAQ

## General

**Q: What is SNEPPX-Algo?**  
A: A composable, cryptographically-secure AI algorithm pipeline built with security as a first principle.

**Q: Is this a production-ready framework?**  
A: No — v0.1.x is a research prototype. The architecture is real, the math is sound, but GPU support, distributed training, and formal verification are future work.

**Q: Can I use it as a PyTorch replacement?**  
A: That is not the goal. SNEPPX-Algo explores a different design axis: security in every instruction. Interop with PyTorch via ONNX or custom export may come in v1.0+.

## Build

**Q: What compilers are supported?**  
A: MSVC 2022 (Windows), GCC 11+ (Linux), Clang 14+ (Linux/macOS). C11 required, C++20 optional for S2 obfuscation.

**Q: Why does the build fail on MSVC with C4819?**  
A: Add `/utf-8` to your CMake flags, or ensure all source files are saved as UTF-8 with BOM.

**Q: Do I need Python to build?**  
A: No. The C core has zero dependencies. Python is only needed for the optional pybind11 bindings.

## Architecture

**Q: How does attention work without softmax?**  
A: Standard softmax attention is implemented. Flash Attention v2 and v3 (TMA+WGMMA) are implemented in the opt-in CUDA backend (`SNEPPX_BUILD_CUDA=ON`); CPU-side Flash/linear attention variants remain planned for v1.0.

**Q: What is RoPE and why is it used?**  
A: Rotary Position Embedding encodes relative position through rotation matrices applied to query and key vectors. It enables better length generalization than absolute position encoding.

**Q: Can I use individual components without the full pipeline?**  
A: Yes. Every component (HSS, SER, ARC, NPE, FM, Attention) has a standalone API and can be used independently.

## Security

**Q: Which security layers are implemented?**  
A: All ten phases are complete: S0 (cryptographic core — Ed25519, ChaCha20-Poly1305, SHA-3, BLAKE3, Argon2id), S1 (secure memory — guard pages, canaries, ASLR), S2 (obfuscation engine — control flow, string encryption, virtualization), S3 (behavioral monitor), S4 (network security — TLS 1.3, Noise, QUIC, mTLS, WireGuard, NIDS), S5 (AI sanitization — semantic injection, jailbreak, model-inversion, watermarking), S6 (key vault), S7 (secure updates — TUF, A/B, TPM), S8 (formal verification — TLA+, LTL, symbolic exec), S9 (penetration testing — CVE scanner, fuzzing, supply-chain audit).

**Q: Is the cryptography audited?**  
A: Not yet. S0 passes standard test vectors (NIST, RFC 8439, RFC 8032) but has not undergone third-party audit.

## Contributing

**Q: How do I submit a patch?**  
A: Email `git format-patch` output to algoSNEPPX@gmail.com. No pull requests. See CONTRIBUTING.md.

**Q: Can I become a maintainer?**  
A: Not at this time. The project uses a BDFL governance model.

## Roadmap

**Q: When will GPU support arrive?**  
A: A CUDA backend is already implemented (opt-in, `SNEPPX_BUILD_CUDA=ON`) covering GEMM, elementwise, layernorm, softmax, AdamW, memory pool, and RNG. Additional backends (Vulkan, TPU, HTTP, ZK, Metal, oneAPI) are opt-in reference-compute paths, OFF by default.

**Q: When will the model be trainable?**  
A: CPU training works today — `test_train_integration` builds a full HSS train graph, runs tape backward, and steps Adam deterministically (loss drops >90% over 10 steps in v0.9.7.890e).

**Q: Will you release pretrained weights?**  
A: Yes — a 7B parameter model is planned for v1.0 (2028).
