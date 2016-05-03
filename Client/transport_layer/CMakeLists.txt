cmake_minimum_required(VERSION 2.8)

project(CLIENT)

set(XML_PARSER_SRC  
			../../DataClasses/Stanza.cpp)

set(XML_PARSER_DIR 	
			 ../../DataClasses)

set(SOURCES_LIB 
			source/client.cpp)

set(HEADERS_DIR 
			header)

set(PORTS_DIR
			../port)

find_package(Boost COMPONENTS system REQUIRED)

if(NOT Boost_FOUND)
	message(SEND_ERROR "Failed to find boost::system.")
	return()
else()
	include_directories(${Boost_INCLUDE_DIRS})
endif()

add_definitions(-std=c++11)

include_directories(${XML_PARSER_DIR})
include_directories(${HEADERS_DIR})
include_directories(${PORTS_DIR})

add_library(CLIENT STATIC ${SOURCES_LIB})

add_library(XML_PARSER STATIC ${XML_PARSER_SRC})
#set_property(TARGET XML_PARSER PROPERTY
#			 IMPORTED_LOCATION ${XML_PARSER_DIR})
#add_subdirectory(DataClasses)

#add_executable(main main.cpp)

target_link_libraries(CLIENT XML_PARSER ${Boost_LIBRARIES} pthread boost_system)
