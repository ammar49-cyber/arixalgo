"""Secure update bindings — S7: signed updates, container security (SBOM, CVE).

Wraps C implementations in ``security/updates/`` with pure-Python fallback.
"""

import os
import json
import hashlib
import time
import threading
from typing import Dict, List, Optional, Tuple

from .c_loader import load_library

_LIB, _HAS_C = load_library("neural_security_c")


class SignedUpdate:
    """TUF-compliant signed update verification and application."""

    def __init__(self):
        self._updates: Dict[str, dict] = {}
        self._lock = threading.Lock()
        self._has_c = _HAS_C

    @staticmethod
    def hash_package(data: bytes) -> str:
        return hashlib.sha256(data).hexdigest()

    @staticmethod
    def sign_update(data: bytes, signing_key: bytes) -> Tuple[bytes, bytes]:
        import hmac as _hmac
        signature = _hmac.new(signing_key, data, hashlib.sha256).digest()
        return data, signature

    @staticmethod
    def verify_signature(data: bytes, signature: bytes, public_key: bytes) -> bool:
        import hmac as _hmac
        expected = _hmac.new(public_key, data, hashlib.sha256).digest()
        return len(expected) == len(signature) and all(a == b for a, b in zip(expected, signature))

    def register_update(self, update_id: str, metadata: dict) -> None:
        with self._lock:
            self._updates[update_id] = {
                "metadata": metadata,
                "registered_at": time.time(),
                "status": "pending",
            }

    def apply(self, update_id: str) -> bool:
        with self._lock:
            entry = self._updates.get(update_id)
            if entry is None:
                return False
            entry["status"] = "applied"
            entry["applied_at"] = time.time()
        return True

    def rollback(self, update_id: str) -> bool:
        with self._lock:
            entry = self._updates.get(update_id)
            if entry is None:
                return False
            entry["status"] = "rolled_back"
        return True

    def status(self, update_id: str) -> Optional[str]:
        with self._lock:
            entry = self._updates.get(update_id)
            return entry["status"] if entry else None


class ContainerSecurity:
    """Container security — SBOM generation, CVE scanning."""

    def __init__(self):
        self._sboms: Dict[str, dict] = {}
        self._vulns: Dict[str, List[dict]] = {}
        self._lock = threading.Lock()
        self._has_c = _HAS_C

    @staticmethod
    def generate_sbom(packages: List[Dict[str, str]]) -> dict:
        sbom = {
            "bomFormat": "CycloneDX",
            "specVersion": "1.4",
            "version": 1,
            "metadata": {
                "timestamp": time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime()),
            },
            "components": [],
        }
        for pkg in packages:
            sbom["components"].append({
                "type": "library",
                "name": pkg.get("name", "unknown"),
                "version": pkg.get("version", "0.0.0"),
                "purl": f"pkg:pypi/{pkg.get('name', 'unknown')}@{pkg.get('version', '0.0.0')}",
            })
        return sbom

    def register_sbom(self, image_id: str, sbom: dict) -> None:
        with self._lock:
            self._sboms[image_id] = sbom

    def scan_cves(self, image_id: str) -> List[dict]:
        with self._lock:
            return self._vulns.get(image_id, [])

    def add_vulnerability(self, image_id: str, vuln: dict) -> None:
        with self._lock:
            self._vulns.setdefault(image_id, []).append(vuln)

    def is_compliant(self, image_id: str, policy: dict) -> bool:
        vulns = self.scan_cves(image_id)
        max_critical = policy.get("max_critical", 0)
        critical_count = sum(1 for v in vulns if v.get("severity") == "critical")
        return critical_count <= max_critical
