cmake_minimum_required(VERSION 4.0)
project(FastSpinLock CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(COVERAGE "Build tests with gcov/lcov coverage support" OFF)

if(COVERAGE)
    # Coverage builds need -O0; --coverage flags are applied in tests/CMakeLists.txt
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g -Wall -Wextra -Werror -pthread" CACHE STRING "CXX flags" FORCE)
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wextra -Werror -pthread" CACHE STRING "CXX flags" FORCE)
endif()

add_subdirectory(tests)
