cmake_minimum_required(VERSION 2.8)

project(MAIN)

set(SOURCES_EXE 
			main.cpp)

set(PORTS_DIR
             port)

set(HEADERS_DIR 
             transport_layer/header)

add_definitions(-std=c++11)

include_directories(${HEADERS_DIR})
include_directories(${PORTS_DIR})
include_directories(user_interface)
include_directories(transport_layer)

add_executable(main ${SOURCES_EXE})

add_subdirectory(user_interface)
add_subdirectory(transport_layer)

target_link_libraries(main user_interface transport_layer pthread)
