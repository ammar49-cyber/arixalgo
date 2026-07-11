# NPE algorithm CUDA extensions

file(GLOB SNEPPX_NPE_CUDA_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cu)

add_library(neural_npe_cuda STATIC ${SNEPPX_NPE_CUDA_SRC})

target_include_directories(neural_npe_cuda PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(neural_npe_cuda PUBLIC CUDA::cudart)

target_link_libraries(neural_architecture_layer PRIVATE neural_npe_cuda)
