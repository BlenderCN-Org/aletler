# CMake project file for sound editor
# Changxi Zheng (cxz@cs.columbia.edu)
project(SND_EDIT)

cmake_minimum_required(VERSION 2.8.0 FATAL_ERROR)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

option(BUILD_DEBUG "Turn on the debug mode" OFF)
option(BUILD_TEST "Build unit test cases" ON)

include(sploosh_utils)
include(snd_edit_utils)
include(CMakeDependentOption)

#===================================================================
## Compiler
# set compiler flags for debug/release
if ( BUILD_DEBUG )
    add_definitions(-DDEBUG)
    set(CMAKE_BUILD_TYPE Debug)
else ()
    add_definitions(-Wno-deprecated)
    set(CMAKE_BUILD_TYPE Release)
endif ()

config_compiler_and_linker()

set(BIN_DIR ${CMAKE_CURRENT_BINARY_DIR}/bin)

#===================================================================
## Detect libraries
# boost
set(Boost_ADDITIONAL_VERSIONS "1.51" "1.51.0")
find_package(Boost 1.50 REQUIRED)

# Eigen
find_package(Eigen REQUIRED)

if (CMAKE_CXX_COMPILER MATCHES ".*icpc$")
    # enable MKL if using intel compiler
    set(MKL_LIBS mkl_intel_lp64 mkl_intel_thread mkl_core ${OPENMP_LIB})
    add_definitions(-DUSE_MKL)
    add_definitions(-DEIGEN_USE_MKL_ALL)
endif()

# TBB
find_package(TBB REQUIRED)

#include(snd_edit_external)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}/src
    ${Eigen_INCLUDE_DIR}
    ${Boost_INCLUDE_DIR})

LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

#===================================================================
## build stuff
add_subdirectory(src/tools)
