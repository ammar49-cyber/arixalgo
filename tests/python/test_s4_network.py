"""Tests for s4_network.py — S4 network security."""

import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "..", "bindings", "python"))

from SneppX_ALG.interface_bindings import DDoSMitigation, IdentityManager, TransportSecurity


def test_ddos_allow():
    d = DDoSMitigation(max_connections_per_ip=3)
    for _ in range(3):
        assert d.allow_connection("1.2.3.4")
    assert not d.allow_connection("1.2.3.4")


def test_ddos_different_ips():
    d = DDoSMitigation(max_connections_per_ip=2)
    assert d.allow_connection("1.1.1.1")
    assert d.allow_connection("2.2.2.2")
    assert d.allow_connection("1.1.1.1")
    assert not d.allow_connection("1.1.1.1")
    assert d.allow_connection("2.2.2.2")


def test_ddos_reset_ip():
    d = DDoSMitigation(max_connections_per_ip=1)
    d.allow_connection("1.2.3.4")
    assert not d.allow_connection("1.2.3.4")
    d.reset("1.2.3.4")
    assert d.allow_connection("1.2.3.4")


def test_ddos_reset_all():
    d = DDoSMitigation(max_connections_per_ip=1)
    d.allow_connection("1.2.3.4")
    d.reset()
    assert d.allow_connection("1.2.3.4")


def test_ddos_syn_flood():
    d = DDoSMitigation()
    assert d.is_syn_flood("1.2.3.4", 1001)
    assert not d.is_syn_flood("1.2.3.4", 999)


def test_identity_register():
    im = IdentityManager()
    im.register("device1", b"\x01" * 32, {"location": "dc1"})
    entry = im.lookup("device1")
    assert entry is not None
    assert entry["public_key"] == b"\x01" * 32


def test_identity_lookup_missing():
    im = IdentityManager()
    assert im.lookup("nonexistent") is None


def test_identity_revoke():
    im = IdentityManager()
    im.register("d1", b"\x01" * 32)
    assert im.revoke("d1")
    assert not im.revoke("d1")


def test_transport_cipher_select():
    ts = TransportSecurity()
    ciphers = ["TLS_AES_128_GCM_SHA256", "TLS_CHACHA20_POLY1305_SHA256"]
    selected = ts.select_cipher(ciphers)
    assert selected in ts.supported_ciphers()


def test_transport_min_version():
    ts = TransportSecurity("1.2")
    assert ts.min_version == "1.2"
    ts.min_version = "1.3"
    assert ts.min_version == "1.3"


if __name__ == "__main__":
    test_ddos_allow()
    test_ddos_different_ips()
    test_ddos_reset_ip()
    test_ddos_reset_all()
    test_ddos_syn_flood()
    test_identity_register()
    test_identity_lookup_missing()
    test_identity_revoke()
    test_transport_cipher_select()
    test_transport_min_version()
    print("All s4_network tests passed.")
