cmake_minimum_required(VERSION 2.8)

set(PROJECT
		USER_INTERFACE)

set(HEADERS
		header/Authorization.h
		header/ChatterBox.h
		header/PortController.h)

set(HEADERS_DIR
		header)

set(PORTS_DIR
		../port)

set(SOURCES
		source/Authorization.cpp
		source/ChatterBox.cpp)

set(XML_PARSER_DIR
		../../DataClasses)

set(XML_PARSER_SRC
		../../DataClasses/Stanza.cpp)
		
project(${PROJECT})

include_directories(${HEADERS_DIR})
include_directories(${XML_PARSER_DIR})
include_directories(${PORTS_DIR})

add_definitions(-std=c++11)

find_package(Qt4 REQUIRED)

if(NOT Qt4_FOUND)
	message(SEND_ERROR "Failed to find QT libraries.")
    return()
else()
	include(${QT_USE_FILE})
	qt4_add_resources (QRC_SOURCES ${RESOURCES})
	qt4_wrap_cpp (MOC_SOURCES ${HEADERS})
endif()

source_group ("XML parser Files" FILES ${XML_PARSER_SRC})
source_group ("Header Files" FILES ${HEADERS})
source_group ("Source Files" FILES ${SOURCES})
source_group ("Generated Files" FILES ${MOC_SOURCES})
source_group ("Resource Files" FILES ${QRC_SOURCES})

#add_library(XML_PARSER STATIC ${XML_PARSER_SRC})
add_library(${PROJECT} STATIC ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${QRC_SOURCES} ${XML_PARSER_SRC})
#add_executable (${PROJECT} ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${QRC_SOURCES} ${XML_PARSER_SRC})

#target_link_libraries (${PROJECT} ${XML_PARSER} ${QT_LIBRARIES} pthread)
