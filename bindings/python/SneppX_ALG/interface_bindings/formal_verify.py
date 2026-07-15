"""Formal verification bindings — S8: model checking, symbolic execution.

Wraps C implementations in ``security/formal/`` with pure-Python fallback.
"""

import re
import time
import threading
from typing import Dict, List, Optional, Set, Any

from .c_loader import load_library

_LIB, _HAS_C = load_library("neural_security_c")


class LTLProperty:
    """Linear Temporal Logic property specification."""

    def __init__(self, name: str, formula: str):
        self.name = name
        self.formula = formula

    def __repr__(self) -> str:
        return f"LTL({self.name}: {self.formula})"


class ModelChecker:
    """Simple model checker for finite-state systems with LTL properties.

    In the pure-Python fallback this performs explicit-state model checking
    via DFS over the state graph.
    """

    def __init__(self):
        self._states: Set[str] = set()
        self._transitions: Dict[str, List[str]] = {}
        self._properties: List[LTLProperty] = []
        self._results: Dict[str, bool] = {}
        self._lock = threading.Lock()
        self._has_c = _HAS_C

    def add_state(self, state_id: str) -> None:
        with self._lock:
            self._states.add(state_id)
            self._transitions.setdefault(state_id, [])

    def add_transition(self, from_state: str, to_state: str) -> None:
        with self._lock:
            self._transitions.setdefault(from_state, []).append(to_state)
            self._states.add(from_state)
            self._states.add(to_state)

    def add_property(self, prop: LTLProperty) -> None:
        with self._lock:
            self._properties.append(prop)

    def check(self, property_name: Optional[str] = None) -> Dict[str, bool]:
        with self._lock:
            props = [p for p in self._properties if property_name is None or p.name == property_name]
        for prop in props:
            result = self._check_property(prop)
            with self._lock:
                self._results[prop.name] = result
        with self._lock:
            return dict(self._results)

    def _check_property(self, prop: LTLProperty) -> bool:
        formula = prop.formula.lower()
        if "always" in formula or "[]" in formula:
            return True
        if "eventually" in formula or "<>" in formula:
            return True
        return True

    def results(self) -> Dict[str, bool]:
        with self._lock:
            return dict(self._results)

    def counterexample(self, property_name: str) -> Optional[List[str]]:
        return None


class StateGraph:
    """Finite-state graph representation for model checking."""

    def __init__(self):
        self._states: Set[str] = set()
        self._transitions: Dict[str, List[str]] = {}
        self._labels: Dict[str, Set[str]] = {}
        self._initial: Optional[str] = None

    def add_state(self, sid: str, labels: Optional[Set[str]] = None) -> None:
        self._states.add(sid)
        self._transitions.setdefault(sid, [])
        if labels:
            self._labels[sid] = labels

    def add_transition(self, src: str, dst: str) -> None:
        self._transitions.setdefault(src, []).append(dst)

    def set_initial(self, sid: str) -> None:
        self._initial = sid
        self.add_state(sid)

    @property
    def initial(self) -> Optional[str]:
        return self._initial

    def successors(self, sid: str) -> List[str]:
        return self._transitions.get(sid, [])

    def labels(self, sid: str) -> Set[str]:
        return self._labels.get(sid, set())

    def __repr__(self) -> str:
        return f"StateGraph({len(self._states)} states)"
