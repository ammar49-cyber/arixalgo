# Model Zoo - Model configuration and registry
# Part of Phase 1: Config Schema

set(MODEL_ZOO_SOURCES
    model_config.c
)

# Create static library
add_library(neural_model_config STATIC ${MODEL_ZOO_SOURCES})

# Include directories
target_include_directories(neural_model_config PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

# C standard
set_target_properties(neural_model_config PROPERTIES
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
)

# Link dependencies
target_link_libraries(neural_model_config PUBLIC
    neural_core_kernel
)

# Test
if(BUILD_TESTING)
    add_executable(test_model_config tests/test_model_config.c)
    target_link_libraries(test_model_config PRIVATE neural_model_config)
    target_include_directories(test_model_config PRIVATE
        ${CMAKE_SOURCE_DIR}/include
    )
    add_test(NAME test_model_config COMMAND test_model_config)
endif()
