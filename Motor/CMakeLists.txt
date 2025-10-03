#Moteur/Cmake

get_filename_component(TARGET_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# ------- Sources files
# ------- Retrieving all the source files and putting them into a kind of list ------- #

message("[${TARGET_NAME}] Starting source file fetching..")

set(headerRoot "${CMAKE_CURRENT_SOURCE_DIR}/Header")
set(sourceRoot "${CMAKE_CURRENT_SOURCE_DIR}/Source")

### ------- Header files

file(GLOB_RECURSE TARGET_HEADER_FILES 
	${CMAKE_CURRENT_SOURCE_DIR}/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/*.inl
	${CMAKE_CURRENT_SOURCE_DIR}/*.natvis
	${CMAKE_CURRENT_SOURCE_DIR}/.editorconfig
        ${CMAKE_CURRENT_SOURCE_DIR}/*.sln.DotSettings)
list(FILTER TARGET_HEADER_FILES EXCLUDE REGEX ${CMAKE_CURRENT_BINARY_DIR})

### ------- Source (C++) files

file(GLOB_RECURSE TARGET_SOURCE_FILES 
	${CMAKE_CURRENT_SOURCE_DIR}/*.cd
	${CMAKE_CURRENT_SOURCE_DIR}/*.c
	${CMAKE_CURRENT_SOURCE_DIR}/*.cc
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/*.cxx
	${CMAKE_CURRENT_SOURCE_DIR}/*.c++)
list(FILTER TARGET_SOURCE_FILES EXCLUDE REGEX ${CMAKE_CURRENT_BINARY_DIR})


file(GLOB_RECURSE TARGET_DLL_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/DLL/*.dll)
set(BIN_DEPENDENCIES ${TARGET_DLL_FILES} PARENT_SCOPE)


file(GLOB_RECURSE TARGET_LIB_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*/Addon/*.lib)
file(GLOB_RECURSE TARGET_DEBUG_LIB_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*/Debug/*.lib)
file(GLOB_RECURSE TARGET_RELEASE_LIB_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*/Release/*.lib)
list(FILTER TARGET_EXTRA_FILES EXCLUDE REGEX ${CMAKE_CURRENT_BINARY_DIR})


foreach(LIB ${TARGET_DEBUG_LIB_FILES})
	set(TARGET_LIB_FILES ${TARGET_LIB_FILES} debug ${LIB})
endforeach()

foreach(LIB ${TARGET_RELEASE_LIB_FILES})
	set(TARGET_LIB_FILES ${TARGET_LIB_FILES} optimized ${LIB})
endforeach()

# ------- Putting all those files under a common name/variable TARGET_FILES ------- #

set(TARGET_FILES ${TARGET_HEADER_FILES} ${TARGET_SOURCE_FILES})

#source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${TARGET_FILES})


# ------- Executable
# ------- In this part of the code will link everything together ------- #

add_library(
  ${TARGET_NAME}
  STATIC
)

add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    		COMMAND ${CMAKE_COMMAND} -E copy 
		${TARGET_DLL_FILES} ${PROJECT_SOURCE_DIR}/Editor/Assets
		VERBATIM COMMAND_EXPAND_LISTS)

message("[${TARGET_NAME}] The executable is being linked with the project's dependencies..")

### ------- Appending the previous source files into the executable

target_sources(${TARGET_NAME} PRIVATE ${TARGET_FILES} ${EDITORCONFIG_PATH})

### ------- Specifies the include directories so Visual Studio knows 

target_link_libraries(${TARGET_NAME} PRIVATE ${TARGET_LIB_FILES})

target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Header)
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/LibMath/Header)

file(GLOB TARGET_DIRECTORIES LIST_DIRECTORIES true ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/Header/*)

target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/Header)

foreach(DIRECTORY ${TARGET_DIRECTORIES})
    if(IS_DIRECTORY ${DIRECTORY})
	target_include_directories(${TARGET_NAME} PUBLIC ${DIRECTORY})
    else() 
	continue() 
    endif()
endforeach()

if (MSVC)
    add_definitions(/MP)
    target_compile_options(${TARGET_NAME} PRIVATE /bigobj)
endif()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${TARGET_FILES})

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

message("[${TARGET_NAME}] The executable is being linked with the project's dependencies..")


### ------- Since this is the Core we link every single previously compiled sub-project

set(MODERN_LIBRARY ${TARGET_NAME} PARENT_SCOPE)

message("[${TARGET_NAME}] Done.")