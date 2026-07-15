"""Tests for crypto_sign.py — Ed25519, Dilithium, SPHINCS+."""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "..", "bindings", "python"))

from SneppX_ALG.interface_bindings import Ed25519, Dilithium, SphincsPlus


def test_ed25519_keypair():
    sk, pk = Ed25519.keypair()
    assert len(sk) in (32, 64)
    assert len(pk) == 32


def test_ed25519_sign_verify():
    sk, pk = Ed25519.keypair()
    msg = b"hello world"
    sig = Ed25519.sign(sk, msg)
    assert Ed25519.verify(pk, msg, sig)


def test_ed25519_wrong_message():
    sk, pk = Ed25519.keypair()
    msg = b"hello"
    sig = Ed25519.sign(sk, msg)
    assert not Ed25519.verify(pk, b"wrong", sig)


def test_ed25519_seed():
    seed = b"s" * 32
    sk1, pk1 = Ed25519.keypair(seed)
    sk2, pk2 = Ed25519.keypair(seed)
    assert sk1 == sk2
    assert pk1 == pk2


def test_ed25519_empty_message():
    sk, pk = Ed25519.keypair()
    sig = Ed25519.sign(sk, b"")
    assert Ed25519.verify(pk, b"", sig)


def test_dilithium_keypair():
    d = Dilithium(Dilithium.MODE_ML_DSA_44)
    pk, sk = d.keypair()
    assert len(pk) > 0
    assert len(sk) > 0


def test_dilithium_modes():
    for mode in [Dilithium.MODE_ML_DSA_44, Dilithium.MODE_ML_DSA_65, Dilithium.MODE_ML_DSA_87]:
        d = Dilithium(mode)
        pk, sk = d.keypair()
        sig = d.sign(sk, b"test")
        assert d.verify(pk, b"test", sig)


def test_sphincsplus_keypair():
    s = SphincsPlus("128s")
    pk, sk = s.keypair()
    assert len(pk) == 32
    assert len(sk) == 64


def test_sphincsplus_variants():
    for v in ["128s", "128f", "192s", "192f", "256s", "256f"]:
        s = SphincsPlus(v)
        pk, sk = s.keypair()
        sig = s.sign(sk, b"msg")
        assert s.verify(pk, b"msg", sig)


def test_dilithium_sign_verify():
    d = Dilithium()
    pk, sk = d.keypair()
    msg = b"test message for dilithium"
    sig = d.sign(sk, msg)
    assert d.verify(pk, msg, sig)


if __name__ == "__main__":
    test_ed25519_keypair()
    test_ed25519_sign_verify()
    test_ed25519_wrong_message()
    test_ed25519_seed()
    test_ed25519_empty_message()
    test_dilithium_keypair()
    test_dilithium_modes()
    test_sphincsplus_keypair()
    test_sphincsplus_variants()
    test_dilithium_sign_verify()
    print("All crypto_sign tests passed.")
