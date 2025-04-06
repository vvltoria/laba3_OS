cmake_minimum_required(VERSION 3.10)
project(Lab3Synchronization)

set(CMAKE_CXX_STANDARD 14)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/release-1.12.1.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include_directories(src)

set(SRC_FILES
    src/main.cpp
    src/marker.cpp
    src/helpers.cpp
)
add_executable(lab3 ${SRC_FILES})

enable_testing()
set(TEST_SOURCES
    tests/test_helpers.cpp
    src/helpers.cpp
)
add_executable(runTests ${TEST_SOURCES})
target_link_libraries(runTests gtest_main pthread)
add_test(NAME runTests COMMAND runTests)
