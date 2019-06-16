cmake_minimum_required (VERSION 3.12)

# Version/options stuff
set (TRAVIS_BUILD_NUMBER "$ENV{TRAVIS_BUILD_NUMBER}")

if ("${TRAVIS_BUILD_NUMBER}" STREQUAL "")
	set (TRAVIS_BUILD_NUMBER "1")
endif ("${TRAVIS_BUILD_NUMBER}" STREQUAL "")

option (BUILD_TYPE_RELEASE "Is build type Release")

if (BUILD_TYPE_RELEASE)
	set (CMAKE_BUILD_TYPE "Release")
	set (LIB_SUBDIR ".")
else (BUILD_TYPE_RELEASE)
	set (CMAKE_BUILD_TYPE "Debug")
	set (LIB_SUBDIR "Debug")
endif (BUILD_TYPE_RELEASE)

# set (CMAKE_CXX_COMPILER "g++")
set (CMAKE_CXX_COMPILER "clang++-6.0")
# set (CMAKE_CXX_COMPILER "icpc")

# Project stuff
project (bulkmt
	VERSION 0.0.${TRAVIS_BUILD_NUMBER}
	DESCRIPTION "Homework 10 Bulks of commands multithreaded implementation"
	HOMEPAGE_URL "https://github.com/edwdmkrv/bulkmt"
	LANGUAGES CXX
)

find_package (GTest REQUIRED)
find_package (Threads REQUIRED)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	set (FILESYSTEM_LIBRARY "-lstdc++fs")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	set (FILESYSTEM_LIBRARY "-lstdc++fs")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
	set (FILESYSTEM_LIBRARY "-lstdc++fs")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	set (FILESYSTEM_LIBRARY "")
else (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	message (FATAL_ERROR "Unsupported compiler ${CMAKE_CXX_COMPILER_ID}")
endif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

configure_file (version.hpp.in version.hpp)

set (CMAKE_VERBOSE_MAKEFILE OFF)

# Library stuff
add_subdirectory (lib)

# Program stuff
add_subdirectory (src)

# CTest stuff
add_subdirectory (tst)
include (CTest)

# CPack stuff
add_subdirectory (pkg)
