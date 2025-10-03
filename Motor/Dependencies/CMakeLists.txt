#Moteur/Dependencies/Cmake


get_filename_component(TARGET_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

if(${CMAKE_CURRENT_SOURCE_DIR} PATH_EQUAL ${CMAKE_SOURCE_DIR})
	# Create project if build directly
	set(CMAKE_CXX_STANDARD 20)
	set(CMAKE_CXX_STANDARD_REQUIRED True)

	project(${TARGET_NAME} LANGUAGES CXX)
endif()

# ------- Sources files
# ------- Retrieving all the source files and putting them into a kind of list ------- #

message("[${TARGET_NAME}] Starting source file fetching..")

### ------- Header files

file(GLOB_RECURSE TARGET_HEADER_FILES 
	${CMAKE_CURRENT_SOURCE_DIR}/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/*.inl)
list(FILTER TARGET_HEADER_FILES EXCLUDE REGEX ${CMAKE_CURRENT_BINARY_DIR})

### ------- Source (C++) files

file(GLOB_RECURSE TARGET_SOURCE_FILES 
	${CMAKE_CURRENT_SOURCE_DIR}/*.c
	${CMAKE_CURRENT_SOURCE_DIR}/*.cc # C with classe
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/*.cxx
	${CMAKE_CURRENT_SOURCE_DIR}/*.c++)
list(FILTER TARGET_SOURCE_FILES EXCLUDE REGEX ${CMAKE_CURRENT_BINARY_DIR})

file(GLOB_RECURSE TARGET_DLL_FILES 
	${CMAKE_CURRENT_SOURCE_DIR}/*.dll)

### ------- Addon (.lib) files

file(GLOB_RECURSE TARGET_LIB_FILES 
	${CMAKE_CURRENT_SOURCE_DIR}/Addon/*.lib)
list(FILTER TARGET_EXTRA_FILES EXCLUDE REGEX ${CMAKE_CURRENT_BINARY_DIR})

set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/Header/Lua)

set(LUA_INCLUDE_DIR${CMAKE_CURRENT_SOURCE_DIR}/Header/Lua)
set(LUA_LIBRARIES ${CMAKE_SOURCE_DIR}/Addon)

find_package(Lua51 REQUIRED)


# ------- Putting all those files under a common name/variable TARGET_FILES ------- #

set(TARGET_FILES ${TARGET_HEADER_FILES} ${TARGET_SOURCE_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${TARGET_FILES})


# ------- Static Library
# ------- In this part of the code will link everything together ------- #


# add_library(${TARGET_NAME} STATIC)

ADD_LIBRARY(
  ${TARGET_NAME}
  SHARED IMPORT
)

SET_TARGET_PROPERTIES(
  ${TARGET_NAME}
  IMPORTED_IMPLIB ${TARGET_LIB_FILES}
  IMPORTED_LOCATION ${TARGET_DLL_FILES}
)

INSTALL(
  TARGETS ${TARGET_NAME}
  RUNTIME_DEPENDENCIES
)

# message("[${TARGET_NAME}] The static library is being linked with the project's dependencies..")

### ------- Appending the previous source files into the static library

target_sources(${TARGET_NAME} PRIVATE ${TARGET_FILES})

### ------- Specifies the include directories so Visual Studio knows 

target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Header)

target_link_libraries(${TARGET_NAME} PRIVATE ${TARGET_LIB_FILES})


### ------- Exposing a variable so the next compilation knows about me

set(DEPENDENCIES_LIBRARY ${TARGET_NAME} PARENT_SCOPE)