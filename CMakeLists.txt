cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_TOOLCHAIN_FILE "vcpkg/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING "Vcpkg toolchain file")

project(audio-engine)
enable_testing()
find_package(SndFile CONFIG REQUIRED)

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
)
FetchContent_MakeAvailable(googletest)


# Audio Engine Library

add_library(AudioEngineLib STATIC
    ./src/effects/ffDelay.cpp 
    ./src/dsp/CircularBuffer.cpp 
    ./src/audio/AudioProcessor.cpp 
    ./src/io/AudioFileIO.cpp 
    ./src/audio/BlockUtils.cpp 
    ./src/effects/Gain.cpp 
    ./src/effects/EffectChain.cpp
)

target_include_directories(AudioEngineLib PUBLIC ./src)

# Link library to SndFile
target_link_libraries(AudioEngineLib PUBLIC SndFile::sndfile)

add_executable(mainApp ./src/app/main.cpp)

# Link main app to Audio Engine Library
target_link_libraries(mainApp PRIVATE AudioEngineLib)

# Create a test executable containing test files
add_executable(AudioTests ./src/tests/GainTest.cpp
                        
                        
                        ./src/tests/BlockUtilsTest.cpp
                        ./src/tests/CircularBufferTest.cpp
                        ./src/tests/EffectChainTest.cpp
                        ./src/tests/ffDelayTest.cpp 
                        # ./src/tests/AudioFileIOTest.cpp
)               

# Link the tests to Audio Engine AND Google Test
target_link_libraries(AudioTests PRIVATE AudioEngineLib GTest::gtest_main)

# Tell CMake to treat this executable as a test suite
include(GoogleTest)
gtest_discover_tests(AudioTests)
