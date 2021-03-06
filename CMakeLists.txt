cmake_minimum_required(VERSION 2.8.5)
# Uncomment the lines below for GDB debugging information
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
# Uncomment the lines below to enable more verbose compiler warnings
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall -Wextra -Wno-write-strings")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes")
set(BUILD_SHARED_LIBS TRUE)
project(cserial)
set(PROJECT_DESCRIPTION "Cross platform library for serial port.")

# uncomment line below to enable debug messages
#add_definitions(-DDEBUG)

# Windows requires special attributes that specify storage-class information
# in order to indicate that a function or object is imported or exported from
# a DLL. The MSVC depends on these attributes to create the .lib which is
# required for linking.
if(WIN32)
	add_definitions(-Dcserial_EXPORTS)
endif(WIN32)

set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)

# configure a header file to pass CMake settings to source code
configure_file(
	"${PROJECT_SOURCE_DIR}/version.rc.in"
	"${PROJECT_BINARY_DIR}/version.rc"
)

# add the binary tree to the search path for include files for main.h
include_directories("${PROJECT_BINARY_DIR}/src")
include_directories("${PROJECT_SOURCE_DIR}/src")

add_subdirectory(src)
add_subdirectory(test)
