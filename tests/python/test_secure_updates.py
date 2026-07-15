"""Tests for secure_updates.py — S7 secure updates."""

import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "..", "bindings", "python"))

from SneppX_ALG.interface_bindings import SignedUpdate, ContainerSecurity


def test_signed_update_hash():
    h = SignedUpdate.hash_package(b"test data")
    assert len(h) == 64


def test_signed_update_sign_verify():
    data, sig = SignedUpdate.sign_update(b"important", b"\x01" * 32)
    assert SignedUpdate.verify_signature(b"important", sig, b"\x01" * 32)


def test_signed_update_verify_wrong_key():
    data, sig = SignedUpdate.sign_update(b"data", b"\x01" * 32)
    assert not SignedUpdate.verify_signature(b"data", sig, b"\x02" * 32)


def test_signed_update_register():
    su = SignedUpdate()
    su.register_update("v2", {"version": "2.0", "size": 1234})
    assert su.status("v2") == "pending"


def test_signed_update_apply():
    su = SignedUpdate()
    su.register_update("v1", {})
    assert su.apply("v1")
    assert su.status("v1") == "applied"


def test_signed_update_rollback():
    su = SignedUpdate()
    su.register_update("v1", {})
    su.apply("v1")
    assert su.rollback("v1")
    assert su.status("v1") == "rolled_back"


def test_signed_update_missing():
    su = SignedUpdate()
    assert not su.apply("missing")
    assert not su.rollback("missing")
    assert su.status("missing") is None


def test_container_sbom():
    cs = ContainerSecurity()
    sbom = cs.generate_sbom([
        {"name": "numpy", "version": "1.26.0"},
        {"name": "requests", "version": "2.31.0"},
    ])
    assert sbom["bomFormat"] == "CycloneDX"
    assert len(sbom["components"]) == 2


def test_container_sbom_register():
    cs = ContainerSecurity()
    sbom = cs.generate_sbom([{"name": "pkg", "version": "1.0"}])
    cs.register_sbom("image:latest", sbom)
    assert cs.scan_cves("image:latest") == []


def test_container_vulnerability():
    cs = ContainerSecurity()
    cs.add_vulnerability("img:v1", {"id": "CVE-2024-0001", "severity": "critical"})
    vulns = cs.scan_cves("img:v1")
    assert len(vulns) == 1
    assert vulns[0]["id"] == "CVE-2024-0001"


def test_container_compliance():
    cs = ContainerSecurity()
    cs.add_vulnerability("img:v2", {"id": "CVE-1", "severity": "critical"})
    assert not cs.is_compliant("img:v2", {"max_critical": 0})
    assert cs.is_compliant("img:v2", {"max_critical": 1})


if __name__ == "__main__":
    test_signed_update_hash()
    test_signed_update_sign_verify()
    test_signed_update_verify_wrong_key()
    test_signed_update_register()
    test_signed_update_apply()
    test_signed_update_rollback()
    test_signed_update_missing()
    test_container_sbom()
    test_container_sbom_register()
    test_container_vulnerability()
    test_container_compliance()
    print("All secure_updates tests passed.")
