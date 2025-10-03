#Root CMAKE

cmake_minimum_required (VERSION 3.27 FATAL_ERROR)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED True)

get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${PROJECT_NAME} LANGUAGES CXX)

set(EDITORCONFIG_PATH ${CMAKE_CURRENT_SOURCE_DIR}/.editorconfig)

if(MSVC)
	message("[${PROJECT_NAME}] You're running in Visual Studio.")
else()
	message("[${PROJECT_NAME}] You're not running in Visual Studio.")
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory(Motor)
add_subdirectory(Editor)

if (MSVC)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Editor)

endif()
