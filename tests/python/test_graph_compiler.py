"""Tests for Graph Compiler."""

import pytest
import numpy as np


def _import_graph_compiler():
    """Import graph compiler modules fresh to avoid pytest caching."""
    import importlib
    import sys

    # Remove cached modules
    for mod in list(sys.modules.keys()):
        if mod.startswith("SneppX_ALG.interface_bindings.graph_compiler"):
            del sys.modules[mod]
    from SneppX_ALG.interface_bindings.graph_compiler import (
        OpType,
        GraphNode,
        Graph,
        FusionPattern,
        GraphCompiler,
        compile_graph,
        create_example_graph,
    )

    return (
        OpType,
        GraphNode,
        Graph,
        FusionPattern,
        GraphCompiler,
        compile_graph,
        create_example_graph,
    )


def test_op_type_enum():
    """Test OpType enum values."""
    OpType, _, _, _, _, _, _ = _import_graph_compiler()
    assert OpType.MATMUL.value == "matmul"
    assert OpType.ADD.value == "add"
    assert OpType.RELU.value == "relu"
    assert OpType.GELU.value == "gelu"
    assert OpType.SIGMOID.value == "sigmoid"
    assert OpType.SILU.value == "silu"
    assert OpType.LAYERNORM.value == "layernorm"
    assert OpType.CONV2D.value == "conv2d"


def test_graph_node_creation():
    """Test GraphNode creation."""
    OpType, GraphNode, _, _, _, _, _ = _import_graph_compiler()
    node = GraphNode(
        op_type=OpType.MATMUL,
        inputs=["a", "b"],
        outputs=["c"],
        attributes={"transpose_b": True},
        name="matmul_1",
    )
    assert node.op_type == OpType.MATMUL
    assert node.inputs == ["a", "b"]
    assert node.outputs == ["c"]
    assert node.attributes["transpose_b"] is True
    assert node.name == "matmul_1"


def test_graph_creation():
    """Test Graph creation."""
    _, GraphNode, Graph, _, _, _, _ = _import_graph_compiler()
    graph = Graph()
    assert graph.nodes == []
    assert graph.inputs == []
    assert graph.outputs == []
    assert graph.constants == {}
    assert graph.shapes == {}
    assert graph.dtypes == {}


def test_constant_folding_add():
    """Test constant folding for ADD."""
    OpType, GraphNode, Graph, _, GraphCompiler, _, _ = _import_graph_compiler()
    graph = Graph()
    graph.constants = {
        "a": np.array([1.0, 2.0]),
        "b": np.array([3.0, 4.0]),
    }
    graph.shapes = {"a": [2], "b": [2]}
    graph.dtypes = {"a": "float32", "b": "float32"}

    graph.nodes = [
        GraphNode(op_type=OpType.ADD, inputs=["a", "b"], outputs=["c"], name="add_1")
    ]
    graph.shapes["c"] = [2]
    graph.dtypes["c"] = "float32"

    compiler = GraphCompiler()
    compiled = compiler.compile(graph)

    assert "c" in compiled.constants
    np.testing.assert_allclose(compiled.constants["c"], [4.0, 6.0])


def test_constant_folding_relu():
    """Test constant folding for RELU."""
    OpType, GraphNode, Graph, _, GraphCompiler, _, _ = _import_graph_compiler()
    graph = Graph()
    graph.constants = {"a": np.array([-1.0, 2.0, -3.0])}
    graph.shapes = {"a": [3]}
    graph.dtypes = {"a": "float32"}

    graph.nodes = [
        GraphNode(op_type=OpType.RELU, inputs=["a"], outputs=["b"], name="relu_1")
    ]
    graph.shapes["b"] = [3]
    graph.dtypes["b"] = "float32"

    compiler = GraphCompiler()
    compiled = compiler.compile(graph)

    assert "b" in compiled.constants
    np.testing.assert_allclose(compiled.constants["b"], [0.0, 2.0, 0.0])


def test_constant_folding_gelu():
    """Test constant folding for GELU."""
    OpType, GraphNode, Graph, _, GraphCompiler, _, _ = _import_graph_compiler()
    graph = Graph()
    graph.constants = {"a": np.array([-1.0, 0.0, 1.0])}
    graph.shapes = {"a": [3]}
    graph.dtypes = {"a": "float32"}

    graph.nodes = [
        GraphNode(op_type=OpType.GELU, inputs=["a"], outputs=["b"], name="gelu_1")
    ]
    graph.shapes["b"] = [3]
    graph.dtypes["b"] = "float32"

    compiler = GraphCompiler()
    compiled = compiler.compile(graph)

    assert "b" in compiled.constants
    # GELU(-1) = -1 * Phi(-1) = -1 * 0.158655 = -0.158655
    # GELU(0) = 0
    # GELU(1) = 1 * Phi(1) = 0.841345
    expected = np.array([-0.158655, 0.0, 0.841345], dtype=np.float32)
    np.testing.assert_allclose(compiled.constants["b"], expected, rtol=1e-3)


def test_constant_folding_transpose():
    """Test constant folding for TRANSPOSE."""
    OpType, GraphNode, Graph, _, GraphCompiler, _, _ = _import_graph_compiler()
    graph = Graph()
    graph.constants = {"a": np.arange(6).reshape(2, 3)}
    graph.shapes = {"a": [2, 3]}
    graph.dtypes = {"a": "float32"}

    graph.nodes = [
        GraphNode(
            op_type=OpType.TRANSPOSE,
            inputs=["a"],
            outputs=["b"],
            attributes={"perm": [1, 0]},
            name="transpose_1",
        )
    ]
    graph.shapes["b"] = [3, 2]
    graph.dtypes["b"] = "float32"

    compiler = GraphCompiler()
    compiled = compiler.compile(graph)

    assert "b" in compiled.constants
    expected = np.array([[0, 3], [1, 4], [2, 5]])
    np.testing.assert_allclose(compiled.constants["b"], expected)


def test_constant_folding_reshape():
    """Test constant folding for RESHAPE."""
    OpType, GraphNode, Graph, _, GraphCompiler, _, _ = _import_graph_compiler()
    graph = Graph()
    graph.constants = {"a": np.arange(6).astype(np.float32)}
    graph.shapes = {"a": [6]}
    graph.dtypes = {"a": "float32"}

    graph.nodes = [
        GraphNode(
            op_type=OpType.RESHAPE,
            inputs=["a"],
            outputs=["b"],
            attributes={"shape": [2, 3]},
            name="reshape_1",
        )
    ]
    graph.shapes["b"] = [2, 3]
    graph.dtypes["b"] = "float32"

    compiler = GraphCompiler()
    compiled = compiler.compile(graph)

    assert "b" in compiled.constants
    expected = np.arange(6).reshape(2, 3)
    np.testing.assert_allclose(compiled.constants["b"], expected)


def test_constant_folding_concat():
    """Test constant folding for CONCAT."""
    OpType, GraphNode, Graph, _, GraphCompiler, _, _ = _import_graph_compiler()
    graph = Graph()
    graph.constants = {
        "a": np.array([1.0, 2.0]),
        "b": np.array([3.0, 4.0]),
    }
    graph.shapes = {"a": [2], "b": [2]}
    graph.dtypes = {"a": "float32", "b": "float32"}

    graph.nodes = [
        GraphNode(
            op_type=OpType.CONCAT,
            inputs=["a", "b"],
            outputs=["c"],
            attributes={"axis": 0},
            name="concat_1",
        )
    ]
    graph.shapes["c"] = [4]
    graph.dtypes["c"] = "float32"

    compiler = GraphCompiler()
    compiled = compiler.compile(graph)

    assert "c" in compiled.constants
    expected = np.array([1.0, 2.0, 3.0, 4.0])
    np.testing.assert_allclose(compiled.constants["c"], expected)


def test_dead_code_elimination():
    """Test dead code elimination."""
    OpType, GraphNode, Graph, _, GraphCompiler, _, _ = _import_graph_compiler()
    graph = Graph()
    graph.inputs = ["input"]
    graph.outputs = ["output"]
    graph.shapes = {"input": [4], "w1": [4, 8], "w2": [4, 8], "unused": [4, 8]}
    graph.dtypes = {
        "input": "float32",
        "w1": "float32",
        "w2": "float32",
        "unused": "float32",
    }

    graph.nodes = [
        GraphNode(
            op_type=OpType.MATMUL,
            inputs=["input", "w1"],
            outputs=["out1"],
            name="matmul_1",
        ),
        GraphNode(
            op_type=OpType.MATMUL,
            inputs=["input", "w2"],
            outputs=["unused_out"],
            name="matmul_2",
        ),  # Unused
    ]
    graph.outputs = ["out1"]

    compiler = GraphCompiler()
    compiled = compiler.compile(graph)

    assert len(compiled.nodes) == 1
    assert compiled.nodes[0].name == "matmul_1"


def test_fusion_matmul_bias_relu():
    """Test fusion of MatMul + Bias + ReLU."""
    OpType, GraphNode, Graph, _, GraphCompiler, _, _ = _import_graph_compiler()
    graph = Graph()
    graph.inputs = ["input"]
    graph.outputs = ["output"]
    graph.shapes = {
        "input": [1, 512],
        "weight": [512, 1024],
        "bias": [1024],
    }
    graph.dtypes = {"input": "float32", "weight": "float32", "bias": "float32"}

    # Add bias to constants so it can be fused
    graph.constants = {
        "weight": np.random.randn(512, 1024).astype(np.float32),
        "bias": np.random.randn(1024).astype(np.float32),
    }
    graph.shapes["weight"] = [512, 1024]
    graph.shapes["bias"] = [1024]
    graph.dtypes["weight"] = "float32"
    graph.dtypes["bias"] = "float32"

    graph.nodes = [
        GraphNode(
            op_type=OpType.MATMUL,
            inputs=["input", "weight"],
            outputs=["mm_out"],
            name="matmul_1",
        ),
        GraphNode(
            op_type=OpType.ADD,
            inputs=["mm_out", "bias"],
            outputs=["add_out"],
            name="add_1",
        ),
        GraphNode(
            op_type=OpType.RELU, inputs=["add_out"], outputs=["output"], name="relu_1"
        ),
    ]
    graph.shapes["output"] = [1, 1024]
    graph.dtypes["output"] = "float32"

    compiler = GraphCompiler()
    compiled = compiler.compile(graph)

    # Should be fused into single node
    assert len(compiled.nodes) == 1
    fused = compiled.nodes[0]
    assert fused.op_type == OpType.MATMUL
    assert fused.attributes.get("fused_bias") is True
    assert fused.attributes.get("fused_activation") == "relu"


def test_fusion_matmul_bias_gelu():
    """Test fusion of MatMul + Bias + GELU."""
    OpType, GraphNode, Graph, _, GraphCompiler, _, _ = _import_graph_compiler()
    graph = Graph()
    graph.inputs = ["input"]
    graph.outputs = ["output"]
    graph.shapes = {
        "input": [1, 512],
        "weight": [512, 1024],
        "bias": [1024],
    }
    graph.dtypes = {"input": "float32", "weight": "float32", "bias": "float32"}

    # Add bias to constants
    graph.constants = {
        "weight": np.random.randn(512, 1024).astype(np.float32),
        "bias": np.random.randn(1024).astype(np.float32),
    }
    graph.shapes["weight"] = [512, 1024]
    graph.shapes["bias"] = [1024]
    graph.dtypes["weight"] = "float32"
    graph.dtypes["bias"] = "float32"

    graph.nodes = [
        GraphNode(
            op_type=OpType.MATMUL,
            inputs=["input", "weight"],
            outputs=["mm_out"],
            name="matmul_1",
        ),
        GraphNode(
            op_type=OpType.ADD,
            inputs=["mm_out", "bias"],
            outputs=["add_out"],
            name="add_1",
        ),
        GraphNode(
            op_type=OpType.GELU, inputs=["add_out"], outputs=["output"], name="gelu_1"
        ),
    ]
    graph.shapes["output"] = [1, 1024]
    graph.dtypes["output"] = "float32"

    compiler = GraphCompiler()
    compiled = compiler.compile(graph)

    assert len(compiled.nodes) == 1
    fused = compiled.nodes[0]
    assert fused.op_type == OpType.MATMUL
    assert fused.attributes.get("fused_bias") is True
    assert fused.attributes.get("fused_activation") == "gelu"


def test_fusion_conv_bias_relu():
    """Test fusion of Conv2D + Bias + ReLU."""
    OpType, GraphNode, Graph, _, GraphCompiler, _, _ = _import_graph_compiler()
    graph = Graph()
    graph.inputs = ["input"]
    graph.outputs = ["output"]
    graph.shapes = {
        "input": [1, 3, 32, 32],
        "weight": [64, 3, 3, 3],
        "bias": [64],
    }
    graph.dtypes = {"input": "float32", "weight": "float32", "bias": "float32"}

    # Add bias to constants
    graph.constants = {
        "weight": np.random.randn(64, 3, 3, 3).astype(np.float32),
        "bias": np.random.randn(64).astype(np.float32),
    }
    graph.shapes["weight"] = [64, 3, 3, 3]
    graph.shapes["bias"] = [64]
    graph.dtypes["weight"] = "float32"
    graph.dtypes["bias"] = "float32"

    graph.nodes = [
        GraphNode(
            op_type=OpType.CONV2D,
            inputs=["input", "weight"],
            outputs=["conv_out"],
            attributes={"strides": [1, 1], "pads": [1, 1, 1, 1]},
            name="conv_1",
        ),
        GraphNode(
            op_type=OpType.ADD,
            inputs=["conv_out", "bias"],
            outputs=["add_out"],
            name="add_1",
        ),
        GraphNode(
            op_type=OpType.RELU, inputs=["add_out"], outputs=["output"], name="relu_1"
        ),
    ]
    graph.shapes["output"] = [1, 64, 32, 32]
    graph.dtypes["output"] = "float32"

    compiler = GraphCompiler()
    compiled = compiler.compile(graph)

    assert len(compiled.nodes) == 1
    fused = compiled.nodes[0]
    assert fused.op_type == OpType.CONV2D
    assert fused.attributes.get("fused_bias") is True
    assert fused.attributes.get("fused_activation") == "relu"


def test_compile_example_graph():
    """Test compiling the example graph."""
    OpType, _, _, _, GraphCompiler, compile_graph, create_example_graph = (
        _import_graph_compiler()
    )
    graph = create_example_graph()
    compiled = compile_graph(graph)

    assert len(compiled.nodes) == 1
    fused = compiled.nodes[0]
    assert fused.op_type == OpType.MATMUL
    assert fused.attributes.get("fused_bias") is True
    assert fused.attributes.get("fused_activation") == "relu"


def test_graph_compiler_register_pattern():
    """Test registering custom fusion pattern."""
    OpType, _, _, FusionPattern, GraphCompiler, _, _ = _import_graph_compiler()
    compiler = GraphCompiler()
    initial_count = len(compiler.fusion_patterns)

    custom_pattern = FusionPattern(
        name="custom_pattern",
        pattern=[OpType.ADD, OpType.MUL],
        replacement=OpType.ADD,
        condition=lambda g, i: True,
        transform=lambda g, i: [
            GraphNode(op_type=OpType.ADD, inputs=[], outputs=[], name="fused")
        ],
    )

    compiler.add_pattern(custom_pattern)
    assert len(compiler.fusion_patterns) == initial_count + 1


def test_example_graph_structure():
    """Test the example graph structure."""
    OpType, _, _, _, _, _, create_example_graph = _import_graph_compiler()
    graph = create_example_graph()

    assert graph.inputs == ["input"]
    assert graph.outputs == ["output"]
    assert "input" in graph.shapes
    assert "weight" in graph.constants
    assert "bias" in graph.constants
    assert len(graph.nodes) == 3

    # Check node sequence: MatMul -> Add -> ReLU
    assert graph.nodes[0].op_type == OpType.MATMUL
    assert graph.nodes[1].op_type == OpType.ADD
    assert graph.nodes[2].op_type == OpType.RELU


def test_compile_graph_function():
    """Test the compile_graph convenience function."""
    OpType, _, Graph, _, _, compile_graph, create_example_graph = (
        _import_graph_compiler()
    )
    graph = create_example_graph()
    compiled = compile_graph(graph)

    assert isinstance(compiled, Graph)
    assert len(compiled.nodes) == 1
    assert compiled.nodes[0].op_type == OpType.MATMUL
    assert compiled.nodes[0].attributes.get("fused_bias") is True
    assert compiled.nodes[0].attributes.get("fused_activation") == "relu"


if __name__ == "__main__":
    pytest.main([__file__, "-v", "--tb=short"])
