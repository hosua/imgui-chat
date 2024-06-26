# Project Definition
cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_PARALLEL_LEVEL 3)
project(chat-app VERSION 0.1.0)

# Prerequisites
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Source Files
add_executable(${PROJECT_NAME}
	lib/imgui/imgui.cpp
	lib/imgui/imgui_demo.cpp
	lib/imgui/imgui_draw.cpp
	lib/imgui/imgui_tables.cpp
	lib/imgui/imgui_widgets.cpp
	lib/imgui/backends/imgui_impl_sdl2.cpp
	lib/imgui/backends/imgui_impl_sdlrenderer2.cpp
    lib/imgui/backends/imgui_impl_opengl3.cpp

	src/main.cc
	src/client.cc
	src/server.cc
	src/logger.cc
)

# Include Directories
target_include_directories(${PROJECT_NAME} PRIVATE
	${SDL2_INCLUDE_DIRS}
	${OPENGL_INCLUDE_DIR}
	${GLUT_INCLUDE_DIRS}
	${CMAKE_SOURCE_DIR}/lib
	${CMAKE_SOURCE_DIR}/lib/imgui
	${CMAKE_SOURCE_DIR}/lib/imgui/backends)

# Compiler Options
target_compile_options(${PROJECT_NAME} PRIVATE
	-Wall
	-Wextra
	-Wpedantic
	-Wunreachable-code
	-Wunused
	-Wignored-qualifiers
	-Wcast-align
	-Wformat-nonliteral
	-Wformat=2
	-Winvalid-pch
	-Wmissing-declarations
	-Wmissing-format-attribute
	-Wmissing-include-dirs
	-Wredundant-decls
	-Wswitch-default
	-Wodr
	-Wfatal-errors
	-fdiagnostics-color=always
	-pthread
	-pg
	-g)

# Link Libraries (Linux Hacks)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)
target_link_libraries(${PROJECT_NAME} PRIVATE GL)
target_link_libraries(${PROJECT_NAME} PRIVATE GLEW) 	

# Standard Link Libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
	${SDL2_LIBRARIES}
	${OPENGL_INCLUDE_DIR}
	dl
	)

# Post Build Directories
# add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory
# doc)

# Post Build Copying
# add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory
# 	${CMAKE_SOURCE_DIR}/doc/ $<TARGET_FILE_DIR:${PROJECT_NAME}>/doc)

# Packaging
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
