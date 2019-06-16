cmake_minimum_required (VERSION 3.12)

# Project stuff
project (bulkmt_tst
	VERSION 0.0.${TRAVIS_BUILD_NUMBER}
	DESCRIPTION "Homework 7 Bulks of commands implementation tests"
	HOMEPAGE_URL "https://github.com/edwdmkrv/bulk/tree/master/tst"
	LANGUAGES CXX
)

# Test stuff
add_executable (test_bulkmt test_bulkmt.cpp)

set_target_properties (test_bulkmt PROPERTIES
	CXX_STANDARD 17
	CXX_EXTENSIONS OFF
	CXX_STANDARD_REQUIRED ON
	COMPILE_OPTIONS "-pedantic;-Wall;-Wextra"
	LINK_LIBRARIES "${GTEST_LIBRARY};${GTEST_MAIN_LIBRARY};lib;${CMAKE_THREAD_LIBS_INIT}"
)

add_test (NAME Test-bulkmt
	COMMAND test_bulkmt
	WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

include (CTest)
