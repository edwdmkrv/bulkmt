cmake_minimum_required (VERSION 3.12)

# Project stuff
project (bulkmt_src
	VERSION 0.0.${TRAVIS_BUILD_NUMBER}
	DESCRIPTION "Homework 10 Bulks of commands multithreaded implementation program"
	HOMEPAGE_URL "https://github.com/edwdmkrv/bulkmt/tree/master/src"
	LANGUAGES CXX
)

# Program stuff
add_executable (bulkmt bulkmt.cpp)

set_target_properties (bulkmt PROPERTIES
	CXX_STANDARD 17
	CXX_EXTENSIONS OFF
	CXX_STANDARD_REQUIRED ON
	COMPILE_OPTIONS "-pedantic;-Wall;-Wextra"
	LINK_LIBRARIES "lib;${CMAKE_THREAD_LIBS_INIT}"
)

install (TARGETS bulkmt RUNTIME DESTINATION Debug/bin CONFIGURATIONS Debug)
install (TARGETS bulkmt RUNTIME DESTINATION bin CONFIGURATIONS Release)
