"""Tests for formal_verify.py — S8 formal verification."""

import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "..", "bindings", "python"))

from SneppX_ALG.interface_bindings import LTLProperty, ModelChecker, StateGraph


def test_ltl_create():
    p = LTLProperty("safety", "always safe")
    assert p.name == "safety"
    assert p.formula == "always safe"


def test_model_checker_add_states():
    mc = ModelChecker()
    mc.add_state("s1")
    mc.add_state("s2")
    mc.add_transition("s1", "s2")
    mc.add_property(LTLProperty("safe", "always safe"))
    res = mc.check()
    assert "safe" in res
    assert res["safe"]


def test_model_checker_multiple_props():
    mc = ModelChecker()
    mc.add_state("s1")
    mc.add_property(LTLProperty("p1", "always x"))
    mc.add_property(LTLProperty("p2", "eventually y"))
    results = mc.check()
    assert "p1" in results
    assert "p2" in results


def test_model_checker_single_prop():
    mc = ModelChecker()
    mc.add_state("s")
    mc.add_property(LTLProperty("only_one", "formula"))
    results = mc.check("only_one")
    assert len(results) == 1


def test_model_checker_no_props():
    mc = ModelChecker()
    assert mc.check() == {}


def test_model_checker_counterexample():
    mc = ModelChecker()
    assert mc.counterexample("any") is None


def test_state_graph():
    g = StateGraph()
    g.add_state("a", {"init"})
    g.add_state("b", {"accept"})
    g.add_transition("a", "b")
    g.set_initial("a")
    assert g.initial == "a"
    assert "b" in g.successors("a")
    assert "init" in g.labels("a")
    assert "accept" in g.labels("b")


def test_state_graph_empty():
    g = StateGraph()
    assert g.initial is None
    assert g.successors("x") == []


def test_state_graph_labels_default():
    g = StateGraph()
    g.add_state("s")
    assert g.labels("s") == set()


if __name__ == "__main__":
    test_ltl_create()
    test_model_checker_add_states()
    test_model_checker_multiple_props()
    test_model_checker_single_prop()
    test_model_checker_no_props()
    test_model_checker_counterexample()
    test_state_graph()
    test_state_graph_empty()
    test_state_graph_labels_default()
    print("All formal_verify tests passed.")
