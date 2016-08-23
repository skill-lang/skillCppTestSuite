cmake_minimum_required(VERSION 2.8.2)
project(skill_cpp_common)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -funroll-loops -O3 -Wall -pedantic")


################################
# OpenMP
################################

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()


################################
# Build common lib
################################

file(GLOB_RECURSE SOURCE_FILES LIST_DIRECTORIES false skill/*.cpp)

# The resulting library to be used by generated code
ADD_LIBRARY(skill.cpp.common STATIC ${SOURCE_FILES})

set_property(TARGET skill.cpp.common PROPERTY INTERPROCEDURAL_OPTIMIZATION True)
