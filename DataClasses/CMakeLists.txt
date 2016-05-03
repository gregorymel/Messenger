cmake_minimum_required(VERSION 2.8)

project(XML_PARSER)

set(SOURCE_LIB Stanza.cpp)

add_library(XML_PARSER STATIC ${SOURCE_LIB})
add_executable(example example.cpp) 
target_link_libraries(example XML_PARSER)
