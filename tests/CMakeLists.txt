file(GLOB_RECURSE TEST_SRCS ../tests/unit/*.c)
file(GLOB_RECURSE INTEGRATION_SRCS ../tests/integration/*.c)
file(GLOB_RECURSE SECURITY_SRCS ../tests/security/*.c)

macro(add_arix_test test_src link_libs)
    get_filename_component(test_name ${test_src} NAME_WE)
    add_executable(${test_name} ${test_src})
    target_link_libraries(${test_name} ${link_libs})
    target_include_directories(${test_name} PRIVATE
        ${CMAKE_SOURCE_DIR}/src/core/include
        ${CMAKE_SOURCE_DIR}/src/arch/include
        ${CMAKE_SOURCE_DIR}/src/security/c/include
    )
    add_test(NAME ${test_name} COMMAND ${test_name})
endmacro()

foreach(test_src ${TEST_SRCS})
    add_arix_test(${test_src} "arix_core;arix_arch")
endforeach()

foreach(test_src ${INTEGRATION_SRCS})
    add_arix_test(${test_src} "arix_core;arix_arch")
endforeach()

foreach(test_src ${SECURITY_SRCS})
    add_arix_test(${test_src} "arix_security_c;arix_core;arix_arch")
endforeach()
