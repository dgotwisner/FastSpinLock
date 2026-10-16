cmake_minimum_required(VERSION 4.0)
project(FastSpinLock CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(COVERAGE "Build tests with gcov/lcov coverage support" OFF)
option(ASAN "Build tests with AddressSanitizer" OFF)
option(TSAN "Build tests with ThreadSanitizer" OFF)
option(UBSAN "Build tests with UndefinedBehaviorSanitizer" OFF)

# Sanitizers and coverage instrumentation conflict; allow only one at a time.
set(_INSTRUMENTATION_COUNT 0)
if(COVERAGE)
    math(EXPR _INSTRUMENTATION_COUNT "${_INSTRUMENTATION_COUNT} + 1")
endif()
if(ASAN)
    math(EXPR _INSTRUMENTATION_COUNT "${_INSTRUMENTATION_COUNT} + 1")
endif()
if(TSAN)
    math(EXPR _INSTRUMENTATION_COUNT "${_INSTRUMENTATION_COUNT} + 1")
endif()
if(UBSAN)
    math(EXPR _INSTRUMENTATION_COUNT "${_INSTRUMENTATION_COUNT} + 1")
endif()
if(_INSTRUMENTATION_COUNT GREATER 1)
    message(FATAL_ERROR "COVERAGE, ASAN, TSAN, and UBSAN are mutually exclusive; enable only one")
endif()

if(COVERAGE)
    # Coverage builds need -O0; --coverage flags are applied in tests/CMakeLists.txt
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g -Wall -Wextra -Werror -pthread" CACHE STRING "CXX flags" FORCE)
elseif(ASAN OR TSAN OR UBSAN)
    # Sanitizer builds need debug info and modest optimization for useful reports
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O1 -g -Wall -Wextra -Werror -pthread" CACHE STRING "CXX flags" FORCE)
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wextra -Werror -pthread" CACHE STRING "CXX flags" FORCE)
endif()

add_subdirectory(tests)
