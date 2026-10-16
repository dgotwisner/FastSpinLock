cmake_minimum_required(VERSION 4.0)
project(FastSpinLockTests CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

option(COVERAGE "Build tests with gcov/lcov coverage support" OFF)
option(ASAN "Build tests with AddressSanitizer" OFF)
option(TSAN "Build tests with ThreadSanitizer" OFF)

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
if(_INSTRUMENTATION_COUNT GREATER 1)
    message(FATAL_ERROR "COVERAGE, ASAN, and TSAN are mutually exclusive; enable only one")
endif()

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

# --------------------------------------------------------------------
# Coverage (gcov / lcov)
# --------------------------------------------------------------------
if(COVERAGE)
    find_program(GCOV_PATH gcov REQUIRED)
    find_program(LCOV_PATH lcov REQUIRED)
    find_program(GENHTML_PATH genhtml REQUIRED)

    # Prefer -O0 for accurate line coverage; --coverage enables gcov.
    foreach(target gtest gtest_main fastspinlock_tests)
        target_compile_options(${target} PRIVATE
            -O0 -g --coverage
            -fno-inline -fno-inline-small-functions -fno-default-inline
        )
        target_link_options(${target} PRIVATE --coverage)
    endforeach()

    set(COVERAGE_DIR ${CMAKE_BINARY_DIR}/coverage)
    set(COVERAGE_INFO ${COVERAGE_DIR}/coverage.info)
    set(COVERAGE_FILTERED ${COVERAGE_DIR}/coverage_filtered.info)
    set(COVERAGE_HTML ${COVERAGE_DIR}/html)

    # Absolute path to the project root (parent of tests/)
    get_filename_component(PROJECT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)

    add_custom_target(coverage
        COMMAND ${CMAKE_COMMAND} -E make_directory ${COVERAGE_DIR}
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${COVERAGE_HTML}
        COMMAND ${CMAKE_COMMAND} -E rm -f ${COVERAGE_INFO} ${COVERAGE_FILTERED}
        # Reset counters from any previous run
        COMMAND ${LCOV_PATH} --zerocounters --directory ${CMAKE_BINARY_DIR}
            --ignore-errors unused
        # Run the unit tests (produces .gcda files)
        COMMAND $<TARGET_FILE:fastspinlock_tests>
        # Capture coverage data
        # lcov 2.x + gcc 15 can emit inconsistent/mismatch warnings on macros/templates
        COMMAND ${LCOV_PATH} --capture
            --directory ${CMAKE_BINARY_DIR}
            --output-file ${COVERAGE_INFO}
            --rc branch_coverage=1
            --ignore-errors mismatch,gcov,unused,empty,inconsistent
        # Keep only project sources (FastSpinLock.hpp + test sources)
        COMMAND ${LCOV_PATH} --extract ${COVERAGE_INFO}
            "${PROJECT_ROOT}/*"
            --output-file ${COVERAGE_FILTERED}
            --rc branch_coverage=1
            --ignore-errors unused,empty,inconsistent
        # Drop gtest and build-tree noise if any leaked in
        COMMAND ${LCOV_PATH} --remove ${COVERAGE_FILTERED}
            "*/gtest/*"
            "/usr/*"
            --output-file ${COVERAGE_FILTERED}
            --rc branch_coverage=1
            --ignore-errors unused,empty,inconsistent
        # HTML report
        COMMAND ${GENHTML_PATH} ${COVERAGE_FILTERED}
            --output-directory ${COVERAGE_HTML}
            --title "FastSpinLock Coverage"
            --legend
            --show-details
            --branch-coverage
            --ignore-errors source,unmapped,inconsistent
        COMMAND ${CMAKE_COMMAND} -E echo ""
        COMMAND ${CMAKE_COMMAND} -E echo "Coverage report: ${COVERAGE_HTML}/index.html"
        DEPENDS fastspinlock_tests
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Running tests and generating lcov/gcov coverage report"
        VERBATIM
    )
endif()

# --------------------------------------------------------------------
# AddressSanitizer (ASan)
# --------------------------------------------------------------------
if(ASAN)
    foreach(target gtest gtest_main fastspinlock_tests)
        target_compile_options(${target} PRIVATE
            -O1 -g -fsanitize=address -fno-omit-frame-pointer
        )
        target_link_options(${target} PRIVATE -fsanitize=address)
    endforeach()

    add_custom_target(asan
        COMMAND ${CMAKE_COMMAND} -E echo "Running unit tests under AddressSanitizer..."
        COMMAND $<TARGET_FILE:fastspinlock_tests>
        COMMAND ${CMAKE_COMMAND} -E echo "AddressSanitizer run completed successfully"
        DEPENDS fastspinlock_tests
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Running unit tests with AddressSanitizer"
        VERBATIM
    )
endif()

# --------------------------------------------------------------------
# ThreadSanitizer (TSan)
# --------------------------------------------------------------------
if(TSAN)
    foreach(target gtest gtest_main fastspinlock_tests)
        target_compile_options(${target} PRIVATE
            -O1 -g -fsanitize=thread -fno-omit-frame-pointer
        )
        target_link_options(${target} PRIVATE -fsanitize=thread)
    endforeach()

    add_custom_target(tsan
        COMMAND ${CMAKE_COMMAND} -E echo "Running unit tests under ThreadSanitizer..."
        COMMAND $<TARGET_FILE:fastspinlock_tests>
        COMMAND ${CMAKE_COMMAND} -E echo "ThreadSanitizer run completed successfully"
        DEPENDS fastspinlock_tests
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Running unit tests with ThreadSanitizer"
        VERBATIM
    )
endif()
