# NCCL based distributed communication backend.
# NCCL is loaded dynamically at runtime (see nccl.c), so the build does not
# require NCCL headers or libraries to be present.

add_library(neural_net_distributed STATIC nccl.c)

target_include_directories(neural_net_distributed PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)

if(SNEPPX_BUILD_CUDA)
    target_link_libraries(neural_net_distributed PUBLIC CUDA::cudart)
    target_compile_definitions(neural_net_distributed PRIVATE SNEPPX_HAS_CUDA)
endif()

# Expose the distributed backend through the core kernel library.
target_link_libraries(neural_core_kernel PUBLIC neural_net_distributed)
