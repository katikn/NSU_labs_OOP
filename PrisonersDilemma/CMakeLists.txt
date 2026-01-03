cmake_minimum_required(VERSION 3.10)
project(PrisonersDilemma)

set(CMAKE_CXX_STANDARD 17)

# ОСНОВНАЯ ПРОГРАММА
add_executable(dilemma
        main.cpp Game.cpp Strategies.cpp Factory.cpp Config.cpp
)

# ТЕСТЫ (если установлен gtest)
enable_testing()
find_package(GTest)
if(GTest_FOUND)
    add_executable(run_tests
            tests.cpp Game.cpp Strategies.cpp Factory.cpp Config.cpp
    )
    target_link_libraries(run_tests GTest::gtest GTest::gtest_main pthread)
    add_test(NAME AllTests COMMAND run_tests)
endif()