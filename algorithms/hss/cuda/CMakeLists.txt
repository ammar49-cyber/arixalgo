# HSS algorithm CUDA extensions

file(GLOB SNEPPX_HSS_CUDA_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cu)

add_library(neural_hss_cuda STATIC ${SNEPPX_HSS_CUDA_SRC})

target_include_directories(neural_hss_cuda PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(neural_hss_cuda PUBLIC
    CUDA::cudart
    CUDA::cufft
)

target_link_libraries(neural_architecture_layer PUBLIC neural_hss_cuda)
