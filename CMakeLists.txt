cmake_minimum_required(VERSION 3.21)

if(APPLE)
    set(CMAKE_THREAD_LIBS_INIT "-lpthread")
    # set(CMAKE_MACOSX_BUNDLE ON)
endif()

# Set project name here
project(amphora-cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_compile_definitions(DEBUG)
endif()

include_directories(
	config
	include
	src
)

add_subdirectory(src)
file(COPY content DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin)
