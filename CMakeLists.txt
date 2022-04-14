cmake_minimum_required (VERSION 3.8)

project ("MyAssembler")
set(CMAKE_C_STANDARD 99)

MESSAGE(STATUS "operation system is ${CMAKE_SYSTEM}")
 
IF (CMAKE_SYSTEM_NAME MATCHES "Windows")
	MESSAGE(STATUS "current platform: Windows")
	add_subdirectory (src/c/windows)
ELSE ()
	MESSAGE(STATUS "other platform: ${CMAKE_SYSTEM_NAME}")
	add_subdirectory (src/c/linux)
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Windows")
 
MESSAGE(STSTUS "###################################")
