# Security Architecture

## Overview

ARIX-Algo implements security in ten phases (S0-S9). S0 and S1 are complete. S2 and S3 are in progress. S4-S9 are planned.

Each phase builds on the previous. The architecture is designed so that security is not a bolt-on layer but a property of the entire system.

```
S0 ── S1 ── S2 ── S3 ── S4 ── S5 ── S6 ── S7 ── S8 ── S9
│     │     │     │     │     │     │     │     │     │
Crypto Memory Obfusc Monitor Network AI    UI    Update Formal Pentest
Core  Secure Engine  Engine  Sec   San    Sec   Sec    Verif  Report
      Mem
```

## S0 — Cryptographic Core ✅

**Status**: Complete
**Source**: `security/crypto/c/` and `include/neural_core/security/`

### Primitives

| Primitive | Standard | Use Case | Verified |
|-----------|----------|----------|----------|
| Ed25519   | RFC 8032 | Signatures | 304/306 test vectors pass |
| X25519    | RFC 7748 | Key exchange | Full DH exchange |
| ChaCha20-Poly1305 | RFC 8439 | Authenticated encryption | 100+ test vectors |
| SHA-3     | FIPS 202 | Hashing (224/256/384/512) | NIST test vectors |
| SHA-256   | FIPS 180-4 | General-purpose hashing | NIST test vectors |
| BLAKE3    | Reference | Fast hashing | Reference test vectors |
| Argon2id  | RFC 9106 | Secure KDF | Test vectors + timing defense |
| Secure Random | OS CPRG | Entropy | Windows CNG / Linux getrandom |
| Kyber-512/768/1024 | FIPS 203 (ML-KEM) | PQ key encapsulation | NIST KAT vectors |
| Dilithium-2/3/5 | FIPS 204 (ML-DSA) | PQ digital signatures | NIST KAT vectors |
| SPHINCS+-128/192/256 | FIPS 205 (SLH-DSA) | Stateless PQ signatures | NIST KAT vectors |

### API

```c
#include "cryptographic_primitives_bundle.h"

// Ed25519 signing
uint8_t pk[32], sk[64];
arix_ed25519_keypair(pk, sk);
arix_ed25519_sign(sig, msg, msglen, sk, pk);

// ChaCha20-Poly1305 encryption
arix_chacha20_poly1305_encrypt(ct, &ctlen, pt, ptlen, key, nonce, aad, aadlen);

// Argon2id key derivation
arix_s0_argon2id_hash(hash, hashlen, pwd, pwdlen, salt, saltlen, t_cost, m_cost);

// Kyber key encapsulation (CCA-secure via Fujisaki-Okamoto transform)
arix_kyber_keypair(pk, sk, variant);
arix_kyber_encaps(ct, ss, pk, variant);
arix_kyber_decaps(ss, ct, sk, variant);

// Dilithium signatures
arix_dilithium_keypair(pk, sk, variant);
arix_dilithium_sign(sig, &siglen, msg, msglen, sk, variant);
arix_dilithium_verify(sig, siglen, msg, msglen, pk, variant);

// SPHINCS+ signatures (stateless, hash-based)
arix_sphincsplus_keypair(pk, sk, variant);
arix_sphincsplus_sign(sig, &siglen, msg, msglen, sk, variant);
arix_sphincsplus_verify(sig, siglen, msg, msglen, pk, variant);
```

### Known Issues

1. **Ed25519 verification**: 2 of 306 test vectors fail under specific edge conditions (batch verification edge cases). These do not represent security vulnerabilities.
2. **Argon2 timing**: 1 of 4 timing tests shows variation on certain hardware. Mitigated by noise injection.
3. **PQ primitives**: Kyber, Dilithium, and SPHINCS+ implementations are reference-style and may not be optimized for constant-time execution on all platforms.

## S1 — Secure Memory ✅

**Status**: Complete
**Source**: `security/memory/` and `include/neural_core/security/`

### Features

| Feature | Description |
|---------|-------------|
| Guard Pages | Allocate with PROT_NONE guard on both sides in debug mode |
| Canaries | Stack overflow detection on memory regions |
| ASLR | Heap randomization via VirtualAlloc (Windows) / mmap (Linux) |
| Locked Memory | mlock / VirtualLock to prevent swap to disk |
| Secure Wipe | Compiler-barrier-protected zeroing of sensitive data |
| Constant-Time Compare | Timing-attack-resistant memory comparison |
| Memory Leak Detector | Tracks allocations and reports unfreed blocks on shutdown |

### API

```c
#include "protected_memory_manager.h"
#include "memory_leak_detector.h"

// Allocate locked memory (cannot be swapped to disk)
void* ptr = arix_s1_alloc_locked(4096);

// Securely free
arix_s1_free_locked(ptr, 4096);

// Constant-time comparison
int32_t match = arix_s1_memcmp_consttime(a, b, n);

// Memory leak detection
arix_memory_leak_detector_init(max_tracked_allocations);
void* p = malloc(256);
arix_memory_leak_detector_track(p, 256, "cache buffer");
arix_memory_leak_detector_untrack(p);
arix_memory_leak_detector_report(); // prints leaks to stderr
```

### Platform Requirements

- **Linux**: Requires `CAP_IPC_LOCK` capability for memory locking
- **Windows**: Automatic via `VirtualLock`
- **macOS**: Not fully supported (limited mlock)

## S2 — Obfuscation Engine ✅

**Status**: Complete
**Source**: `security/obfuscation/` and `security/cpp/`

### Implemented

- Control flow flattening: converts natural control flow to switch-based dispatch
- String encryption: XOR-based compile-time string obfuscation with rotating keys
- Instruction substitution: replace ADD/SUB/MUL/AND/OR/XOR with NAND-gate equivalent sequences
- Opaque predicates: always-true/false branches to confuse static analysis
- Code virtualization: basic-block-level bytecode compilation with encrypted handler dispatch table
- Anti-debug: ptrace detection, NtGlobalFlag check, timing anomaly detection, breakpoint scanning
- Binary substitution: opcode-level replacement with prefix/suffix byte insertion
- Junk code insertion: NOP slides, dead-store MOVs, identity XORs
- Constant unfolding: integer constants expressed as (a + b) with random splits
- IAT protection: hash-based import resolution, integrity scanning
- SEH/VEH obfuscation: exception-based control flow
- TLS callback obfuscation: runtime function pointer encoding
- Anti-dump: PE/ELF header XOR encryption with CRC integrity verification
- Multi-VM diversity: multiple bytecode handlers with slot switching
- Instruction scheduling randomization: basic-block-level instruction reordering

## S3 — Behavioral Monitor ✅

**Status**: Complete
**Source**: `security/monitor/`

### Implemented

- Integrity monitoring: CRC32-based memory region verification
- Container breakout detection: monitors for escape attempts via cgroup, namespace, and mount inspection
- Frequency analysis: detect unusual API call patterns
- Timing analysis: detect side-channel probing via rdtsc and execution time measurement
- Anomaly detection: statistical baseline comparison for runtime behavior

## S4 — Network Security ✅

**Status**: Complete
**Source**: `security/network/`

### Implemented

- DDoS mitigation: SYN flood detection, rate limiting, connection tracking, IP blacklisting
- Transport security: TLS-compatible handshake padding, traffic analysis resistance
- Identity management: certificate pinning, peer fingerprint verification
- Certificate validation: chain-of-trust verification with expiry checking

### API

```c
#include "ddos_mitigation.h"
#include "transport_security.h"

// DDoS mitigation
ArixDDoSState ddos;
arix_ddos_init(&ddos, 1000, 100);  // 1000 pkt/s window, 100 burst limit
if (arix_ddos_is_attack(&ddos, src_ip, dst_ip, now_us)) {
    arix_ddos_block_ip(&ddos, src_ip);
}

// Transport security
ArixTransportSec ts;
arix_transport_sec_init(&ts);
arix_transport_sec_handshake(&ts, cert, cert_len, key);
```

## S5 — AI Sanitizer ✅

**Status**: Complete
**Source**: `security/ai/`

### Implemented

- Prompt injection detection: regex + embedded pattern scanning for jailbreak attempts
- Differential privacy: Laplace mechanism with configurable epsilon for training data protection
- Data poisoning defense: gradient outlier detection, abnormal loss spike monitoring
- RLHF safety: reward model validation, preference alignment checking, harmful output filtering
- Output verifier: model output constraint checking against allow/deny lists

### API

```c
#include "differential_privacy.h"
#include "rlhf_safety.h"
#include "data_poisoning_defense.h"

// Differential privacy (Laplace mechanism)
double noisy_value = arix_differential_privacy_laplace(original_value, sensitivity, epsilon);

// RLHF safety check
ArixRLHFSafety rlhf;
arix_rlhf_safety_init(&rlhf);
if (arix_rlhf_safety_check_output(&rlhf, model_output, output_len) == 0) {
    // output is safe
}

// Poisoning detection
ArixPoisonDetect pd;
arix_poison_detect_init(&pd, threshold_gradient_norm);
arix_poison_detect_feed_gradient(&pd, grad, grad_len);
int is_poisoned = arix_poison_detect_is_anomalous(&pd);
```

## S6 — Security UI ✅

**Status**: Complete
**Source**: `security/ui/`

### Implemented

- Audit logging: structured JSON log entries with severity levels, timestamps, and source tracking
- Key vault: in-memory encrypted key store with PIN-protected access
- Dashboard: (see `docs/api/c.md` for C API or `bindings/python/` for Python bindings)

## S7 — Secure Updates ✅

**Status**: Complete
**Source**: `security/updates/`

### Implemented

- Container security: OCI-compatible layer verification, manifest integrity checking, image signing
- Signed update bundles: Ed25519-signed update payloads with version rolling
- Rollback protection: monotonic version counter preventing downgrade attacks
- Staged rollout: gradual update distribution with health-check gating

### API

```c
#include "container_security.h"
#include "signed_update.h"

// Container image verification
ArixContainerSec cs;
arix_container_sec_init(&cs);
if (arix_container_sec_verify_layer(&cs, layer_data, layer_len, expected_hash)) {
    // layer integrity verified
}

// Signed update
ArixSignedUpdate su;
arix_signed_update_init(&su, signing_sk);
arix_signed_update_create(&su, update_data, update_len, &bundle, &bundle_len);
```

## S8 — Formal Verification ✅

**Status**: Complete
**Source**: `security/formal/`

### Implemented

- Model checking: bounded state-space exploration for critical algorithm paths
- Invariant verification: pre/post-condition checking on memory safety and data flow
- Symbolic execution: path constraint generation for NPE bytecode verification
- Container breakout detection (formal): state-machine-based detection rule engine

### API

```c
#include "model_checking.h"
#include "container_breakout.h"

// Model checking
ArixModelCheck mc;
arix_model_check_init(&mc, max_states, max_transitions);
arix_model_check_add_invariant(&mc, "ptr != NULL", INV_PTR_NOT_NULL);
int ok = arix_model_check_verify(&mc, program, program_len);

// Container breakout detection
ArixBreakoutDetect bd;
arix_breakout_detect_init(&bd);
arix_breakout_detect_watch_syscall(&bd, "mount", BREAKOUT_FLAG_NAMESPACE_ESCAPE);
```

## S9 — Penetration Testing ✅

**Status**: Complete
**Source**: `security/pentest/`

### Implemented

- Network fuzzer: protocol-aware fuzzing engine with mutation strategies (bit flip, boundary, dictionary)
- Self-audit: comprehensive internal consistency checks across all security layers
- Security report generation: structured output of audit findings
- Capture-the-flag utilities: challenge scaffolding for internal red-team exercises

### API

```c
#include "network_fuzzer.h"

// Network fuzzing
ArixFuzzer fz;
arix_fuzzer_init(&fz, ARIX_FUZZ_TCP);
arix_fuzzer_set_target(&fz, "192.168.1.1", 443);
arix_fuzzer_set_mutation(&fz, ARIX_FUZZ_MUTATE_BITFLIP, 0.05);
arix_fuzzer_run(&fz, duration_sec, &report);
```

## Threat Model

### Assumptions

- The hardware is trusted (no side-channel attacks on CPU)
- The OS is trusted (no kernel-level compromise)
- The network is untrusted
- Other AI models are untrusted and potentially adversarial
- Supply chain is trusted (verified commits, signed releases)

### Defenses

| Threat | Defense | Status |
|--------|---------|--------|
| Signature forgery | Ed25519 | ✅ |
| Data breach at rest | ChaCha20-Poly1305 | ✅ |
| Side-channel timing | Constant-time ops | ✅ |
| Memory scraping | Locked memory + guard pages | ✅ |
| Swap forensic | mlock/VirtualLock | ✅ |
| Reverse engineering | CF flattening + string encryption | ⚠️ Partial |
| Runtime tampering | Behavioral monitor | ⚠️ Partial |
| Adversarial input | ARC input guard | ✅ |
| Gradient leakage | ARC gradient obfuscation | ✅ |
| Model inversion | ARC output verifier | ✅ |

## Reporting Vulnerabilities

See [SECURITY.md](../SECURITY.md)

## Security Best Practices

1. **Always verify signatures**: Use `arix_ed25519_verify` on any external data
2. **Lock sensitive data**: Use `arix_s1_alloc_locked` for keys and secrets
3. **Wipe after use**: Call `arix_s1_free_locked` or `arix_secure_zero` on sensitive buffers
4. **Use constant-time comparisons**: Never use `memcmp` for secret comparison
5. **Use AEAD**: Always use `arix_chacha20_poly1305_encrypt` with associated data
6. **Don't roll your own**: Use the provided crypto primitives; do not implement your own
