cmake_minimum_required(VERSION 4.0)
project(FastSpinLockTests CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

# --------------------------------------------------------------------
# Build gtest manually (CMake 2.8 era)
# --------------------------------------------------------------------
# On Ubuntu: sudo apt-get install libgtest-dev
# This installs sources into /usr/src/gtest but does NOT build them.

include_directories(/usr/src/gtest)
add_library(gtest STATIC /usr/src/gtest/src/gtest-all.cc)
add_library(gtest_main STATIC /usr/src/gtest/src/gtest_main.cc)

# pthread is required for gtest
find_package(Threads REQUIRED)

# --------------------------------------------------------------------
# Test executable
# --------------------------------------------------------------------
add_executable(fastspinlock_tests
    test_fastspinlock.cpp
)

# Include parent directory so FastSpinLock.hpp is visible
include_directories(${PROJECT_SOURCE_DIR}/..)

target_link_libraries(fastspinlock_tests
    gtest
    gtest_main
    ${CMAKE_THREAD_LIBS_INIT}
)

add_test(NAME fastspinlock_tests COMMAND fastspinlock_tests)
