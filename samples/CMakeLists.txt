if(ARIX_BUILD_TESTS)
    add_executable(sample_basic sample_basic.c)
    target_link_libraries(sample_basic PRIVATE arix_core arix_arch arix_security_c)
endif()
