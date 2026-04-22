cmake_minimum_required(VERSION 3.10)

project(audio-engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_TOOLCHAIN_FILE "vcpkg/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING "Vcpkg toolchain file")

find_package(SndFile CONFIG REQUIRED)

# Audio Engine Library
add_library(AudioEngineLib STATIC
    ./src/audio/AudioEngine.cpp
    ./src/effects/ffDelay.cpp
    ./src/dsp/CircularBuffer.cpp
    ./src/audio/AudioProcessor.cpp
    ./src/io/AudioFileIO.cpp
    ./src/audio/BlockUtils.cpp
    ./src/effects/Gain.cpp
    ./src/effects/EffectChain.cpp
)

target_include_directories(AudioEngineLib PUBLIC ./src)

target_link_libraries(AudioEngineLib PUBLIC SndFile::sndfile)

# Main executable
add_executable(mainApp ./src/app/main_engine.cpp)
target_link_libraries(mainApp PRIVATE AudioEngineLib)

# Optional tests
option(BUILD_TESTING "Build unit tests" OFF)

if(BUILD_TESTING)
    enable_testing()

    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
    )
    FetchContent_MakeAvailable(googletest)

    add_executable(AudioTests
        ./src/tests/GainTest.cpp
        ./src/tests/BlockUtilsTest.cpp
        ./src/tests/CircularBufferTest.cpp
        ./src/tests/EffectChainTest.cpp
        ./src/tests/ffDelayTest.cpp
        # ./src/tests/AudioFileIOTest.cpp
    )

    target_link_libraries(AudioTests PRIVATE AudioEngineLib GTest::gtest_main)

    include(GoogleTest)
    gtest_discover_tests(AudioTests)
endif()