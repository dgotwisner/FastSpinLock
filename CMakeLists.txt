cmake_minimum_required(VERSION 4.0)
project(FastSpinLock CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "-O3 -Wall -Wextra -Werror -pthread" CACHE STRING "CXX flags" FORCE)

add_subdirectory(tests)
