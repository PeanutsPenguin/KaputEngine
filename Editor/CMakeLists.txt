#Motor/Editor Cmake

get_filename_component(TARGET_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

# ------- Sources files
# ------- Retrieving all the source files and putting them into a kind of list ------- #

message("[${TARGET_NAME}] Starting source file fetching..")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

### ------- Header files

file(GLOB_RECURSE TARGET_HEADER_FILES 
	${CMAKE_CURRENT_SOURCE_DIR}/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/*.inl
        ${CMAKE_CURRENT_SOURCE_DIR}/.editorconfig
        ${CMAKE_CURRENT_SOURCE_DIR}/*sln.DotSettings)
list(FILTER TARGET_HEADER_FILES EXCLUDE REGEX ${CMAKE_CURRENT_BINARY_DIR})

### ------- Source (C++) files

file(GLOB_RECURSE TARGET_SOURCE_FILES 
	${CMAKE_CURRENT_SOURCE_DIR}/*.c
	${CMAKE_CURRENT_SOURCE_DIR}/*.cc
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/*.cxx
	${CMAKE_CURRENT_SOURCE_DIR}/*.c++)
list(FILTER TARGET_SOURCE_FILES EXCLUDE REGEX ${CMAKE_CURRENT_BINARY_DIR})

### ------- Shader files

file(GLOB_RECURSE TARGET_SHADERS_FILES 
	${CMAKE_CURRENT_SOURCE_DIR}/Assets/*.vert
	${CMAKE_CURRENT_SOURCE_DIR}/Assets/*.frag
	${CMAKE_CURRENT_SOURCE_DIR}/Assets/*.glsl)


# ------- Putting all those files under a common name/variable TARGET_FILES ------- #

set(TARGET_FILES ${TARGET_HEADER_FILES} ${TARGET_SOURCE_FILES} ${TARGET_SHADERS_FILES})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${TARGET_FILES})

# ------- Executable

add_executable(${TARGET_NAME})

# ------- Change working directory ------- #

set_property(TARGET ${TARGET_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Assets")

# ------- Appending the previous source files into the executable

target_sources(${TARGET_NAME} PRIVATE ${TARGET_FILES} ${EDITORCONFIG_PATH})

target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Header)

if (MSVC)
    add_definitions(/MP)
endif()

# ------- Specifies the include directories so Visual Studio knows 

function(add_source_group_dir path)
  set(headerDir "${CMAKE_CURRENT_SOURCE_DIR}/Header/${path}")
  set(sourceDir "${CMAKE_CURRENT_SOURCE_DIR}/Source/${path}")
  file(GLOB headerFiles LIST_DIRECTORIES false "${headerDir}/*")
  file(GLOB sourceFiles LIST_DIRETORIES false "${sourceDir}/*")

  source_group("${path}/Header" FILES ${headerFiles})
  source_group("${path}/Source" FILES ${sourceFiles})

  file(GLOB children RELATIVE "${headerDir}" "${headerDir}/*")
  foreach(child ${children})
    if(IS_DIRECTORY "${headerDir}/${child}")
      add_source_group_dir("${path}/${child}")
    endif()
  endforeach()
endfunction()

file(GLOB children RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/Header" "${CMAKE_CURRENT_SOURCE_DIR}/Header/*")
foreach(child ${children})
  if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Header/${child}")
    add_source_group_dir("${child}")
  endif()
endforeach()


target_link_libraries(${TARGET_NAME} PRIVATE ${MODERN_LIBRARY})

message("[${TARGET_NAME}] Done.")
