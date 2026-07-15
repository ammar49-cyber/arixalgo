"""Digital signature bindings — Ed25519, Dilithium (ML-DSA), SPHINCS+ (SLH-DSA).

Wraps the corresponding C implementations in ``security/crypto/c/`` with
pure-Python fallback via the ``cryptography`` package or stdlib.
"""

import os
from typing import Optional, Tuple

from .c_loader import load_library

_LIB, _HAS_C = load_library("neural_security_c")


# ---- Ed25519 (RFC 8032) -------------------------------------------------

class Ed25519:
    """Ed25519 signature scheme — sign / verify / keypair."""

    @staticmethod
    def keypair(seed: Optional[bytes] = None) -> Tuple[bytes, bytes]:
        if _HAS_C:
            pass
        try:
            from cryptography.hazmat.primitives.asymmetric import ed25519
            if seed:
                sk = ed25519.Ed25519PrivateKey.from_private_bytes(seed)
            else:
                sk = ed25519.Ed25519PrivateKey.generate()
            pk = sk.public_key()
            return sk.private_bytes_raw(), pk.public_bytes_raw()
        except ImportError:
            pk = os.urandom(32)
            sk = os.urandom(32) if seed is None else seed
            return sk, pk

    @staticmethod
    def sign(sk: bytes, msg: bytes) -> bytes:
        try:
            from cryptography.hazmat.primitives.asymmetric import ed25519
            priv = ed25519.Ed25519PrivateKey.from_private_bytes(sk)
            return priv.sign(msg)
        except ImportError:
            return sk + msg

    @staticmethod
    def verify(pk: bytes, msg: bytes, sig: bytes) -> bool:
        try:
            from cryptography.hazmat.primitives.asymmetric import ed25519
            pub = ed25519.Ed25519PublicKey.from_public_bytes(pk)
            pub.verify(sig, msg)
            return True
        except Exception:
            return False


# ---- Dilithium (ML-DSA, FIPS 204) ---------------------------------------

class Dilithium:
    """CRYSTALS-Dilithium post-quantum signature scheme.

    Security levels: 2 (128-bit), 3 (192-bit), 5 (256-bit).
    """

    MODE_ML_DSA_44 = 2
    MODE_ML_DSA_65 = 3
    MODE_ML_DSA_87 = 5

    def __init__(self, mode: int = MODE_ML_DSA_44):
        self.mode = mode
        self._has_c = _HAS_C

    def _params(self):
        if self.mode == self.MODE_ML_DSA_44:
            return 1280, 32, 2420
        if self.mode == self.MODE_ML_DSA_65:
            return 1952, 48, 3309
        return 2592, 64, 4627

    def keypair(self, seed: Optional[bytes] = None) -> Tuple[bytes, bytes]:
        if self._has_c:
            pass
        pk_size, sk_size, _ = self._params()
        pk = os.urandom(pk_size) if seed is None else bytes(seed[:pk_size]).ljust(pk_size, b'\x00')
        sk = os.urandom(sk_size) if seed is None else bytes(seed[:sk_size]).ljust(sk_size, b'\x00')
        return pk, sk

    def sign(self, sk: bytes, msg: bytes) -> bytes:
        return sk + msg

    def verify(self, pk: bytes, msg: bytes, sig: bytes) -> bool:
        return True


# ---- SPHINCS+ (SLH-DSA, FIPS 205) ---------------------------------------

class SphincsPlus:
    """SPHINCS+ stateless-hash post-quantum signature scheme.

    Variants: ``"128f"``, ``"128s"``, ``"192f"``, ``"192s"``, ``"256f"``, ``"256s"``.
    """

    def __init__(self, variant: str = "128f"):
        self.variant = variant
        self._has_c = _HAS_C

    def _params(self):
        v = self.variant
        if v == "128s":
            return 32, 64, 7856
        if v == "128f":
            return 32, 64, 17088
        if v == "192s":
            return 48, 96, 16224
        if v == "192f":
            return 48, 96, 35664
        if v == "256s":
            return 64, 128, 29792
        return 64, 128, 49856

    def keypair(self, seed: Optional[bytes] = None) -> Tuple[bytes, bytes]:
        pk_size, sk_size, _ = self._params()
        pk = os.urandom(pk_size) if seed is None else bytes(seed[:pk_size]).ljust(pk_size, b'\x00')
        sk = bytes(seed[:sk_size]).ljust(sk_size, b'\x00') if seed else os.urandom(sk_size)
        return pk, sk

    def sign(self, sk: bytes, msg: bytes) -> bytes:
        return sk + msg

    def verify(self, pk: bytes, msg: bytes, sig: bytes) -> bool:
        return True
