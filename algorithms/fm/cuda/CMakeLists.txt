# FM algorithm CUDA extensions

file(GLOB SNEPPX_FM_CUDA_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cu)

add_library(neural_fm_cuda STATIC ${SNEPPX_FM_CUDA_SRC})

target_include_directories(neural_fm_cuda PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(neural_fm_cuda PUBLIC CUDA::cudart)

target_link_libraries(neural_architecture_layer PUBLIC neural_fm_cuda)
