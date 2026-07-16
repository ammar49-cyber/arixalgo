# Scripts

Build, CI, and automation scripts for SNEPPX-ALG.

| Script | Description |
|--------|-------------|
| `run_sanitizers.sh` | Linux: ASan/UBSan builds + compute-sanitizer + ctest |
| `run_sanitizers.ps1` | Windows: ASan/UBSan builds + compute-sanitizer + ctest |

Sanitizer scripts perform:
1. CMake configure with `-DSNEPPX_ENABLE_ASAN=ON -DSNEPPX_ENABLE_UBSAN=ON`
2. Build all targets
3. Run ctest with output-on-failure
4. NVIDIA compute-sanitizer (memcheck/racecheck/initcheck) if CUDA available
