# CMake version required
cmake_minimum_required(VERSION 3.10)

# Project name
set(LIBRARY_NAME mikai)
project(${LIBRARY_NAME} LANGUAGES C VERSION 4.10 DESCRIPTION "library to manage a MyKey")

# C11 standard
set(CMAKE_C_STANDARD 11)

# Set release if unset
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Include headers and sources to install
include_directories(include)
include(GNUInstallDirs)
add_subdirectory(src)
add_subdirectory(include)

# Public API
set_target_properties(mikai PROPERTIES PUBLIC_HEADER include/mikai.h)

# Optimization flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pipe")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2 -s")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_RELEASE} -Wall -Wextra -pipe")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 -s")
