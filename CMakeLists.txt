cmake_minimum_required(VERSION 3.10)

project(audio-engine)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Find libsndfile using PkgConfig (Standard for Linux/Pop!_OS)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SNDFILE REQUIRED IMPORTED_TARGET sndfile)

include(FetchContent)
FetchContent_Declare(
    kissfft
    GIT_REPOSITORY https://github.com/mborgerding/kissfft.git
    GIT_TAG master
)
set(KISSFFT_DATATYPE float CACHE STRING "" FORCE)
set(KISSFFT_TOOLS OFF CACHE BOOL "" FORCE)
set(KISSFFT_TEST OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(kissfft)

# 2. Audio Engine Library
add_library(AudioEngineLib STATIC
    ./src/audio/AudioEngine.cpp
    ./src/effects/ffDelay.cpp
    ./src/dsp/CircularBuffer.cpp
    ./src/audio/AudioProcessor.cpp
    ./src/io/AudioFileIO.cpp
    ./src/audio/BlockUtils.cpp
    ./src/effects/Gain.cpp
    ./src/effects/EffectChain.cpp
    ./src/effects/STFTProcessor.cpp
)

# Tell the library where its own headers are
target_include_directories(AudioEngineLib PUBLIC ./src)

# Link the library to sndfile
target_link_libraries(AudioEngineLib PUBLIC PkgConfig::SNDFILE kissfft)

# 3. Main executable
add_executable(mainApp ./src/app/main_engine.cpp)
target_link_libraries(mainApp PRIVATE AudioEngineLib)

# 4. Optional tests
option(BUILD_TESTING "Build unit tests" ON) # Default to ON for dev

if(BUILD_TESTING)
    enable_testing()

    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.14.0  # Using a stable version tag instead of a commit hash
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    # For Windows compatibility (if you ever switch back), but harmless on Linux
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    add_executable(AudioTests
        ./src/tests/GainTest.cpp
        ./src/tests/BlockUtilsTest.cpp
        ./src/tests/CircularBufferTest.cpp
        ./src/tests/EffectChainTest.cpp
        ./src/tests/ffDelayTest.cpp
    )

    # Link against your lib and gtest_main (gtest_main provides the main() function for tests)
    target_link_libraries(AudioTests PRIVATE AudioEngineLib gtest_main)

    include(GoogleTest)
    gtest_discover_tests(AudioTests)
endif()