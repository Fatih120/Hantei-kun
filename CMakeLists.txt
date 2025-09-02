cmake_minimum_required(VERSION 3.13...3.19)
project(hanteikun VERSION 2.4 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" )
set (EXE "hantei-kun" )

execute_process(
  COMMAND git rev-parse --short HEAD
  OUTPUT_VARIABLE GIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
add_definitions(-DGIT_COMMIT_HASH="${GIT_HASH}")

include(glad)
include(imgui)
add_subdirectory(glm)
add_subdirectory(tinyalloc)

add_executable(${EXE} WIN32
	src/main.cpp
	src/main_frame.cpp
	src/main_pane.cpp
	src/right_pane.cpp
	src/box_pane.cpp
	src/render.cpp
	src/vao.cpp
	src/shader.cpp
	src/texture.cpp
	src/filedialog.cpp
	src/framestate.cpp
	src/framedata.cpp
	src/framedata_load.cpp
	src/framedata_save.cpp
	src/cg.cpp
	src/misc.cpp
	src/imgui_utils.cpp
	src/test.cpp
	src/ini.cpp
	src/parts.cpp
	src/clock.cpp
	src/image_write.cpp
	res/res.rc
)

target_include_directories(${EXE} PRIVATE ".")
target_compile_definitions(${EXE} PRIVATE UNICODE _UNICODE WIN32_LEAN_AND_MEAN HA6GUIVERSION="${CMAKE_PROJECT_VERSION}u")

find_package(OpenGL REQUIRED)
target_link_libraries(${EXE} PRIVATE tinyalloc imgui glad OpenGL::GL glm winmm)

if(MINGW)
	message(status "With -municode")
	target_link_options(${EXE} PRIVATE -municode)
endif()

