# SER algorithm CUDA extensions

file(GLOB SNEPPX_SER_CUDA_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cu)

add_library(neural_ser_cuda STATIC ${SNEPPX_SER_CUDA_SRC})

target_include_directories(neural_ser_cuda PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(neural_ser_cuda PUBLIC CUDA::cudart)

target_link_libraries(neural_architecture_layer PRIVATE neural_ser_cuda)
