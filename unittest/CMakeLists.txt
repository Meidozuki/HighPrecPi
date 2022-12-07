# 'Google_test' is the subproject name
project(Google_tests)

# 'lib' is the folder with Google Test sources
add_subdirectory(googletest-release-1.12.1)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

# 'Google_Tests_run' is the target name
#add_executable(Google_Tests_run test_big_int.cpp)
#target_link_libraries(Google_Tests_run gtest gtest_main)

function(add_gtest_exe exe src)
    add_executable(${exe} ${src})
    target_link_libraries(${exe} gtest gtest_main)
#    target_link_libraries(${exe} Raster_src_lib)
endfunction()

add_gtest_exe(test_bigint test_big_int-deque.cpp)
add_gtest_exe(test_bigint_vec test_big_int-vector.cpp)