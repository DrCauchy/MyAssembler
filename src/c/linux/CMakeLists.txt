

aux_source_directory (. SRC_LIST)

include_directories (${PROJECT_SOURCE_DIR}/include)
# include_directories (../include)

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Wall -O3")

add_executable (MyAssembler ${SRC_LIST})

set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)