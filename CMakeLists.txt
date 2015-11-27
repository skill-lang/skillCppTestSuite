cmake_minimum_required(VERSION 3.3)
project(skill_c___testsuite)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -pedantic")

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