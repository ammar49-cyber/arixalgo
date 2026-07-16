# Examples

Example programs demonstrating SNEPPX-ALG functionality.

## Basic Examples

| Example | Description |
|---------|-------------|
| `arc_demo` | Adversarial Robustness Certification — PGD attack simulation |
| `fm_demo` | Fractal Memory — distributed memory orchestration |
| `hss_demo` | Hierarchical State Space — sequence modeling |
| `npe_demo` | Neural Programming Engine — program compilation and execution |
| `obf_demo` | Code Obfuscation — S2 layer demonstration |

## Advanced Examples (`examples/advanced/`)

Advanced use cases including multi-node training, custom architectures,
and security-hardened inference pipelines.

## Building

```bash
cmake --build build --config Release --target arc_demo
./build/Release/arc_demo
```
