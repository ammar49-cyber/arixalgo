# ARC algorithm CUDA extensions

file(GLOB SNEPPX_ARC_CUDA_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cu)

add_library(neural_arc_cuda STATIC ${SNEPPX_ARC_CUDA_SRC})

target_include_directories(neural_arc_cuda PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(neural_arc_cuda PUBLIC CUDA::cudart)

target_link_libraries(neural_architecture_layer PUBLIC neural_arc_cuda)
