cmake_minimum_required(VERSION 3.23)
project(DS_pi)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS -O2)

set(SRC_CPP
    main.cpp bigint-deque.cpp fixed_high_prec.cpp)

add_executable(DS_pi ${SRC_CPP})

add_subdirectory(unittest)


if(WIN32)
    set(STRING_CXX_FLAGS "-D_UNICODE")
    foreach(src ${SRC_CPP})
        set_source_files_properties(${src} PROPERTIES
                COMPILE_FLAGS "${STRING_CXX_FLAGS}"
                )
    endforeach()
endif(WIN32)