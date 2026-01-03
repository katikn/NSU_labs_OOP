cmake_minimum_required(VERSION 3.29)
project(SoundProcessor)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(LIB_SOURCES
        SoundProcessorApp.cpp
        WavHeader.cpp
        Exceptions.cpp
        ConfigParser.cpp
        ConverterFactory.cpp
        converters/MuteConverter.cpp
        converters/MixConverter.cpp
        converters/EchoConverter.cpp
        WavStreamReader.cpp
        WavStreamReader.h
        WavStreamWriter.cpp
        WavStreamWriter.h
)

set(HEADERS
        SoundProcessorApp.h
        WavHeader.h
        Exceptions.h
        ConfigParser.h
        ConverterFactory.h
        converters/Converter.h
        converters/MuteConverter.h
        converters/MixConverter.h
        converters/EchoConverter.h
        WavStreamReader.cpp
        WavStreamReader.h
        WavStreamWriter.cpp
        WavStreamWriter.h
)

include_directories(include)

add_executable(sound_processor main.cpp ${LIB_SOURCES} ${HEADERS})

enable_testing()
find_package(GTest)
if(GTest_FOUND)
    add_executable(run_tests tests.cpp ${LIB_SOURCES} ${HEADERS}
            WavStreamReader.cpp
            WavStreamReader.h
            WavStreamWriter.cpp
            WavStreamWriter.h)
    target_link_libraries(run_tests GTest::gtest GTest::gtest_main pthread)
    add_test(NAME AllTests COMMAND run_tests)
endif()

target_compile_options(sound_processor PRIVATE
        -Wall
        -Wextra
        -pedantic
        -O2
)
