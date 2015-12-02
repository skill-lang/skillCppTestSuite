cmake_minimum_required(VERSION 2.8.2)
project(skill_cpp_testsuite)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -pedantic")

################################
# skill c++ common
################################
ADD_SUBDIRECTORY(lib/skill.cpp.common)
include_directories(lib/skill.cpp.common)

################################
# GTest
################################
ADD_SUBDIRECTORY(lib/gtest)
enable_testing()
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

################################
# Unit Tests
################################

#implementation
file(GLOB_RECURSE SOURCE_FILES LIST_DIRECTORIES false src/*.cpp)
#actual tests
file(GLOB_RECURSE TEST_FILES LIST_DIRECTORIES false test/*.cpp)

add_executable(runTests ${SOURCE_FILES} ${TEST_FILES})

# Link test executable against gtest & gtest_main
target_link_libraries(runTests skill.cpp.common gtest gtest_main)
add_test(runTests runTests)

set_property(TARGET runTests PROPERTY INTERPROCEDURAL_OPTIMIZATION True)
