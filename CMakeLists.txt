cmake_minimum_required (VERSION 2.8)
project (sson)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
	message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsigned-char")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L../boost_1_60_0/stage/lib")

IF(NOT CMAKE_BUILD_TYPE)
	SET(CMAKE_BUILD_TYPE Debug FORCE)
ENDIF()

set(sson_VERSION_MAJOR 1)
set(sson_VERSION_MINOR 0)
 
configure_file(
	"${PROJECT_SOURCE_DIR}/Config.h.ini"
	"${PROJECT_BINARY_DIR}/Config.h"
)
 
include_directories(
	"${PROJECT_BINARY_DIR}"
	sson_lib/inc/
	../boost_1_60_0
)

find_library(BOOST boost_system HINTS ../boost_1_60_0/stage/lib)

file(GLOB SSON_SRC
	sson_lib/inc/sson.h
	sson_lib/src/sson.cpp
) 

file(GLOB TCPSERVER_SRC
	tcpserver/ring.h
	tcpserver/async_ring.h
	tcpserver/tcpserver.h
	tcpserver/tcpserver.cpp
) 

set(EXECUTABLE_OUTPUT_PATH .)

add_executable(sson_server tcpserver/sson_server.cpp ${TCPSERVER_SRC} ${SSON_SRC})
target_link_libraries(sson_server ${BOOST})
