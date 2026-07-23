# Model Registry - Model registration and discovery
# Part of Phase 1: Model Registry

set(REGISTRY_SOURCES
    registry.c
)

# Create static library
add_library(neural_model_registry STATIC ${REGISTRY_SOURCES})

# Include directories
target_include_directories(neural_model_registry PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

# C standard
target_compile_options(neural_model_registry PRIVATE /std:c11)

# Link dependencies
target_link_libraries(neural_model_registry PUBLIC
    neural_core_kernel
    neural_model_config
)

# Test
if(BUILD_TESTING)
    add_executable(test_model_config tests/test_model_config.c)
    target_link_libraries(test_model_config PRIVATE neural_model_config)
    target_include_directories(test_model_config PRIVATE
        ${CMAKE_SOURCE_DIR}/include
    )
    add_test(NAME test_model_config COMMAND test_model_config)

    add_executable(test_model_registry tests/test_model_registry.c)
    target_link_libraries(test_model_registry PRIVATE neural_model_registry neural_model_config)
    target_link_directories(test_model_registry PRIVATE
        ${CMAKE_BINARY_DIR}/algorithms/model_zoo/${CMAKE_CFG_INTDIR}
    )
    target_include_directories(test_model_registry PRIVATE
        ${CMAKE_SOURCE_DIR}/include
    )
    add_test(NAME test_model_registry COMMAND test_model_registry)
endif()