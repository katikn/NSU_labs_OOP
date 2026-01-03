cmake_minimum_required(VERSION 3.14)
project(BitArrayProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
FetchContent_MakeAvailable(googletest)

add_executable(run_tests
        main.cpp
        tests/test.cpp
        src/bitarray.cpp
        main.cpp
        main.cpp
)

target_include_directories(run_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(run_tests PRIVATE gtest gtest_main pthread)

enable_testing()
add_test(NAME BitArrayTests COMMAND run_tests)
