"""Tests for ONNX export/import functionality."""

import json
import tempfile
import os
import numpy as np
import pytest
from SneppX_ALG.interface_bindings.onnx_export import (
    OnnxNode,
    OnnxTensor,
    OnnxInitializer,
    OnnxGraph,
    OnnxModel,
    OnnxExporter,
    OnnxImporter,
    DTYPE_TO_ONNX,
    ONNX_TO_DTYPE,
    onnx_validate,
    onnx_version_compatible,
    upgrade_opset,
    export_onnx,
    import_onnx,
    SNEPPX_TO_ONNX_OP,
    ONNX_OP_REGISTRY,
    TensorRTExporter,
)


def test_onnx_node():
    """Test OnnxNode creation and serialization."""
    node = OnnxNode(
        op_type="MatMul",
        inputs=["a", "b"],
        outputs=["out"],
        name="matmul_1",
        attributes={"transA": 0, "transB": 1},
    )
    d = node.to_dict()
    assert d["op_type"] == "MatMul"
    assert d["inputs"] == ["a", "b"]
    assert d["outputs"] == ["out"]
    assert d["name"] == "matmul_1"
    assert d["attributes"]["transA"] == 0


def test_onnx_tensor():
    """Test OnnxTensor creation and serialization."""
    tensor = OnnxTensor("input", "float32", [1, 3, 224, 224])
    d = tensor.to_dict()
    assert d["name"] == "input"
    assert d["dtype"] == DTYPE_TO_ONNX["float32"]
    assert d["shape"] == [1, 3, 224, 224]


def test_onnx_initializer():
    """Test OnnxInitializer creation and serialization."""
    data = np.random.randn(64, 3, 3, 3).astype(np.float32)
    init = OnnxInitializer("weight", data, "float32")
    d = init.to_dict()
    assert d["name"] == "weight"
    assert d["dtype"] == DTYPE_TO_ONNX["float32"]
    assert d["dims"] == [64, 3, 3, 3]


def test_onnx_graph():
    """Test OnnxGraph construction."""
    graph = OnnxGraph("test_model")

    # Add input
    inp = OnnxTensor("input", "float32", [1, 3, 224, 224])
    graph.add_input(inp)

    # Add output
    out = OnnxTensor("output", "float32", [1, 1000])
    graph.add_output(out)

    # Add node
    node = OnnxNode("Conv", ["input", "weight"], ["output"])
    graph.add_node(node)

    # Add initializer
    weight_data = np.random.randn(64, 3, 3, 3).astype(np.float32)
    init = OnnxInitializer("weight", weight_data, "float32")
    graph.add_initializer(init)

    d = graph.to_dict()
    assert d["name"] == "test_model"
    assert len(d["inputs"]) == 1
    assert len(d["outputs"]) == 1
    assert len(d["nodes"]) == 1
    assert len(d["initializers"]) == 1


def test_onnx_model():
    """Test OnnxModel construction."""
    graph = OnnxGraph("model")
    inp = OnnxTensor("x", "float32", [1, 3, 224, 224])
    out = OnnxTensor("y", "float32", [1, 1000])
    graph.add_input(inp)
    graph.add_output(out)

    model = OnnxModel(graph)
    d = model.to_dict()
    assert d["ir_version"] == 9
    assert d["producer_name"] == "SNEPPX"
    assert "graph" in d


def test_onnx_exporter():
    """Test OnnxExporter basic functionality."""
    exporter = OnnxExporter()

    # Add input
    exporter.add_input("input", [1, 3, 224, 224], "float32")

    # Add some nodes
    exporter.add_conv("input", "weight", "bias", strides=(2, 2), pads=(1, 1, 1, 1))
    exporter.add_batch_norm("conv_out", "bn_scale", "bn_bias", "bn_mean", "bn_var")
    exporter.add_relu("bn_out")

    # Add output
    exporter.add_output("output", [1, 64, 112, 112])

    # Export to dict
    model = exporter.build_model()
    d = model.to_dict()

    assert "graph" in d
    assert len(d["graph"]["nodes"]) >= 3
    assert len(d["graph"]["inputs"]) == 1
    assert len(d["graph"]["outputs"]) == 1


def test_onnx_exporter_helpers():
    """Test OnnxExporter helper methods."""
    exporter = OnnxExporter()
    exporter.add_input("x", [1, 3, 224, 224])

    # Test various helpers
    exporter.add_relu("x")
    exporter.add_gelu("x")
    exporter.add_sigmoid("x")
    exporter.add_softmax("x", axis=1)
    exporter.add_batch_norm("x", "scale", "bias", "mean", "var")
    exporter.add_layer_norm("x", "weight", "bias")
    exporter.add_dropout("x", 0.5)
    exporter.add_max_pool("x", (3, 3), strides=(2, 2))
    exporter.add_avg_pool("x", (2, 2))
    exporter.add_gemm("a", "b", "c")
    exporter.add_concat(["a", "b", "c"], axis=1)
    exporter.add_reshape("x", [-1, 256])
    exporter.add_transpose("x", [0, 2, 1, 3])
    exporter.add_squeeze("x", [0])
    exporter.add_unsqueeze("x", [0])
    exporter.add_gather("x", "indices", axis=0)
    exporter.add_reduce_sum("x", [1, 2], keepdims=1)
    exporter.add_reduce_mean("x", [1], keepdims=0)

    outputs = exporter.add_split("x", [10, 20, 30], axis=1)
    assert len(outputs) == 3


def test_onnx_importer():
    """Test OnnxImporter basic functionality."""
    importer = OnnxImporter()

    # Create a simple model and export
    exporter = OnnxExporter()
    exporter.add_input("input", [1, 3, 224, 224])
    exporter.add_conv("input", "weight", "bias")
    exporter.add_output("output", [1, 64, 224, 224])
    model = exporter.build_model()

    # Export to JSON
    import tempfile

    with tempfile.NamedTemporaryFile(mode="w", suffix=".json", delete=False) as f:
        json.dump(model.to_dict(), f)
        path = f.name

    try:
        # Import back
        data = importer.load(path)
        assert "initializers" in data
        assert "nodes" in data
        assert "inputs" in data
        assert "outputs" in data
    finally:
        import os

        os.unlink(path)


def test_dtype_mappings():
    """Test dtype mappings."""
    assert DTYPE_TO_ONNX["float32"] == 1
    assert DTYPE_TO_ONNX["float16"] == 10
    assert DTYPE_TO_ONNX["int32"] == 6
    assert DTYPE_TO_ONNX["int64"] == 7
    assert DTYPE_TO_ONNX["bool"] == 9

    assert ONNX_TO_DTYPE[1] == "float32"
    assert ONNX_TO_DTYPE[10] == "float16"


def test_op_registry():
    """Test ONNX operator registry."""
    assert "Conv" in ONNX_OP_REGISTRY
    assert "MatMul" in ONNX_OP_REGISTRY
    assert "Add" in ONNX_OP_REGISTRY
    assert "Relu" in ONNX_OP_REGISTRY
    assert ONNX_OP_REGISTRY["Conv"]["inputs"] == 2
    assert ONNX_OP_REGISTRY["Relu"]["outputs"] == 1


def test_sneppx_to_onnx_mapping():
    """Test SNEPPX to ONNX operator mapping."""
    assert SNEPPX_TO_ONNX_OP["conv2d"] == "Conv"
    assert SNEPPX_TO_ONNX_OP["matmul"] == "MatMul"
    assert SNEPPX_TO_ONNX_OP["add"] == "Add"
    assert SNEPPX_TO_ONNX_OP["mul"] == "Mul"
    assert SNEPPX_TO_ONNX_OP["relu"] == "Relu"
    assert SNEPPX_TO_ONNX_OP["gelu"] == "Gelu"
    assert SNEPPX_TO_ONNX_OP["conv2d"] == "Conv"


def test_onnx_validation():
    """Test ONNX model validation."""
    graph = OnnxGraph("test")
    inp = OnnxTensor("x", "float32", [1, 3, 224, 224])
    out = OnnxTensor("y", "float32", [1, 1000])
    graph.add_input(inp)
    graph.add_output(out)
    node = OnnxNode("Relu", ["x"], ["y"])
    graph.add_node(node)

    model = OnnxModel(graph)
    valid, errors = onnx_validate(model)
    assert valid
    assert len(errors) == 0


def test_onnx_version_compatible():
    """Test ONNX opset version compatibility."""
    graph = OnnxGraph("test")
    model = OnnxModel(graph, opset_imports=[{"version": 18, "domain": ""}])

    assert onnx_version_compatible(model, 18)
    assert onnx_version_compatible(model, 19)
    assert not onnx_version_compatible(model, 17)


def test_upgrade_opset():
    """Test opset upgrade."""
    graph = OnnxGraph("test")
    model = OnnxModel(graph, opset_imports=[{"version": 15, "domain": ""}])

    upgraded = upgrade_opset(model, 18)
    assert upgraded.opset_imports[0]["version"] == 18


def test_tensorrt_exporter():
    """Test TensorRTExporter placeholder."""
    exporter = TensorRTExporter()
    assert exporter.engine is None
    exporter.export("model.onnx", "model.engine")
    exporter.build_engine(b"dummy_onnx")


def test_export_onnx_function():
    """Test high-level export_onnx function."""
    import tempfile

    with tempfile.NamedTemporaryFile(suffix=".json", delete=False) as f:
        path = f.name

    try:
        export_onnx(
            model=None,
            path=path,
            input_names=["input"],
            output_names=["output"],
            input_shapes=[[1, 3, 224, 224]],
            dynamic_axes={"input": {0: "batch"}, "output": {0: "batch"}},
        )

        import os

        assert os.path.exists(path)
    finally:
        os.unlink(path)


def test_import_onnx_function():
    """Test import_onnx function."""
    exporter = OnnxExporter()
    exporter.add_input("input", [1, 3, 224, 224])
    exporter.add_output("output", [1, 1000])
    model = exporter.build_model()

    import tempfile

    with tempfile.NamedTemporaryFile(mode="w", suffix=".json", delete=False) as f:
        json.dump(model.to_dict(), f)
        path = f.name

    try:
        data = import_onnx(path)
        assert "initializers" in data
        assert "nodes" in data
    finally:
        os.unlink(path)


if __name__ == "__main__":
    pytest.main([__file__, "-v", "--tb=short"])
