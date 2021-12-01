cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

project(mergeVertices)

cmake_policy(SET CMP0053 NEW)
cmake_policy(SET CMP0020 NEW)

add_definitions(-D_USE_MATH_DEFINES)

include_directories ("dependencies/geogram/include/geogram1" "dependencies/geogram/include/GLFW" "inc")

link_directories("dependencies/geogram/lib")

file( GLOB SRC_FILES src/*.cpp )

add_executable(mergeVertices ${SRC_FILES})

target_link_libraries(
    mergeVertices
	geogram
	geogram_gfx
	glfw3dll
)

set( CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_LIST_DIR}/install")

install( TARGETS mergeVertices DESTINATION bin )

file( GLOB GEOGRAM_DLLS dependencies/geogram/bin/*.dll )

install( FILES ${GEOGRAM_DLLS} DESTINATION bin )
