# =============================================================================
# ARIX Algo – Python Bindings (pybind11)
# =============================================================================
cmake_minimum_required(VERSION 3.16)

# ---------------------------------------------------------------------------
# Locate pybind11
# ---------------------------------------------------------------------------
find_package(pybind11 CONFIG QUIET)
if(NOT pybind11_FOUND)
    # Fallback: fetch pybind11 if not installed
    include(FetchContent)
    FetchContent_Declare(
        pybind11
        GIT_REPOSITORY https://github.com/pybind/pybind11.git
        GIT_TAG        v2.12.0
    )
    FetchContent_MakeAvailable(pybind11)
endif()

# ---------------------------------------------------------------------------
# Locate Python 3
# ---------------------------------------------------------------------------
find_package(Python 3.8 REQUIRED COMPONENTS Interpreter Development NumPy)

# ---------------------------------------------------------------------------
# Reference C library targets built by the parent CMake
# ---------------------------------------------------------------------------
# These targets are expected from the top-level CMakeLists.txt build.
# We use find_library / target_link_libraries to link them.

set(ARIX_LIB_DIR "${CMAKE_SOURCE_DIR}/build/lib" CACHE PATH "Directory containing built ARIX libraries")

# ---------------------------------------------------------------------------
# Collect all C source files for direct compilation into the extension
# (Alternative approach: link pre-built static libraries)
# ---------------------------------------------------------------------------

# --- Core tensor ---
file(GLOB_RECURSE ARIX_TENSOR_SRC
    "${CMAKE_SOURCE_DIR}/kernel/tensor/tensor.c"
    "${CMAKE_SOURCE_DIR}/kernel/tensor/internal/*.c"
)

# --- Core memory ---
file(GLOB_RECURSE ARIX_MEMORY_SRC
    "${CMAKE_SOURCE_DIR}/kernel/memory/allocator.c"
    "${CMAKE_SOURCE_DIR}/kernel/memory/internal/*.c"
)

# --- Core thread ---
file(GLOB_RECURSE ARIX_THREAD_SRC
    "${CMAKE_SOURCE_DIR}/kernel/thread/threadpool.c"
)

# --- Autodiff ---
file(GLOB_RECURSE ARIX_AUTODIFF_SRC
    "${CMAKE_SOURCE_DIR}/kernel/autodiff/ops.c"
    "${CMAKE_SOURCE_DIR}/kernel/autodiff/variable.c"
    "${CMAKE_SOURCE_DIR}/kernel/autodiff/tape.c"
)

# --- Optimizer ---
file(GLOB_RECURSE ARIX_OPTIMIZER_SRC
    "${CMAKE_SOURCE_DIR}/kernel/optimizer/optimizer.c"
)

# --- Trainer ---
file(GLOB_RECURSE ARIX_TRAIN_SRC
    "${CMAKE_SOURCE_DIR}/kernel/train/trainer.c"
)

# --- Security (C crypto) ---
file(GLOB_RECURSE ARIX_SECURITY_C_SRC
    "${CMAKE_SOURCE_DIR}/security/crypto/c/*.c"
)

# --- Algorithms ---
file(GLOB_RECURSE ARIX_HSS_SRC
    "${CMAKE_SOURCE_DIR}/algorithms/hss/core/*.c"
)
file(GLOB_RECURSE ARIX_SER_SRC
    "${CMAKE_SOURCE_DIR}/algorithms/ser/core/*.c"
)
file(GLOB_RECURSE ARIX_ARC_SRC
    "${CMAKE_SOURCE_DIR}/algorithms/arc/core/*.c"
)
file(GLOB_RECURSE ARIX_NPE_SRC
    "${CMAKE_SOURCE_DIR}/algorithms/npe/core/*.c"
)
file(GLOB_RECURSE ARIX_FM_SRC
    "${CMAKE_SOURCE_DIR}/algorithms/fm/core/*.c"
)

# --- Aggregate all C sources ---
set(ARIX_ALL_C_SRC
    ${ARIX_TENSOR_SRC}
    ${ARIX_MEMORY_SRC}
    ${ARIX_THREAD_SRC}
    ${ARIX_AUTODIFF_SRC}
    ${ARIX_OPTIMIZER_SRC}
    ${ARIX_TRAIN_SRC}
    ${ARIX_SECURITY_C_SRC}
    ${ARIX_HSS_SRC}
    ${ARIX_SER_SRC}
    ${ARIX_ARC_SRC}
    ${ARIX_NPE_SRC}
    ${ARIX_FM_SRC}
)

# ---------------------------------------------------------------------------
# Build Python extension module
# ---------------------------------------------------------------------------
pybind11_add_module(_arix_c
    MODULE
    "${CMAKE_CURRENT_SOURCE_DIR}/bindings.cpp"
    ${ARIX_ALL_C_SRC}
)

# ---------------------------------------------------------------------------
# Include directories
# ---------------------------------------------------------------------------
target_include_directories(_arix_c PRIVATE
    ${CMAKE_SOURCE_DIR}/include/arix/core
    ${CMAKE_SOURCE_DIR}/include/arix/security
    ${CMAKE_SOURCE_DIR}/include/arix/arch
    ${Python_INCLUDE_DIRS}
)

# Required for MSVC secure CRT warnings
target_compile_definitions(_arix_c PRIVATE
    $<$<CONFIG:Release>:ARIX_S1_SECURE_MEMORY>
    _CRT_SECURE_NO_WARNINGS
)

# ---------------------------------------------------------------------------
# Link Python and NumPy
# ---------------------------------------------------------------------------
target_link_libraries(_arix_c PRIVATE pybind11::headers Python::Python)

if(WIN32)
    target_compile_definitions(_arix_c PRIVATE
        ARIX_CRYPTO_SECURE
        NOMINMAX
    )
endif()

# ---------------------------------------------------------------------------
# Link platform libraries
# ---------------------------------------------------------------------------
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    target_link_libraries(_arix_c PRIVATE rt pthread)
elseif(APPLE)
    target_link_libraries(_arix_c PRIVATE pthread)
endif()

# ---------------------------------------------------------------------------
# Install the compiled module next to the Python package
# ---------------------------------------------------------------------------
install(TARGETS _arix_c
    LIBRARY DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/arix_algo"
    RUNTIME DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/arix_algo"
)

# ---------------------------------------------------------------------------
# Custom target: install the .pyd next to the package source so it can
# be imported directly from the source tree during development.
# ---------------------------------------------------------------------------
add_custom_command(TARGET _arix_c POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        "$<TARGET_FILE:_arix_c>"
        "${CMAKE_CURRENT_SOURCE_DIR}/arix_algo/"
    COMMENT "Copying compiled extension to arix_algo/ package directory"
)
