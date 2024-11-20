
if(NOT DEFINED ENV{COMMONLIB_SSE_FOLDER})
  message(FATAL_ERROR "Missing COMMONLIB_SSE_FOLDER environment variable")
endif()

include(cmake/headerlist.cmake)
include(cmake/sourcelist.cmake)
include(cmake/lib/copyOutputs.cmake)
include(cmake/lib/automaticGameFolderOutput.cmake)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

cmake_minimum_required(VERSION 3.21)
set(AUTHOR_NAME "SkyrimThiago")
set(PRODUCT_NAME "SKSEMenuFramework")
set(BEAUTIFUL_NAME "SKSE Menu Framework")
project(${PRODUCT_NAME} VERSION 2.1.1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)

configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/version.rc
        @ONLY)

set(RESOURCE_FILE ${CMAKE_CURRENT_BINARY_DIR}/version.rc)
set(RESOURCE_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/version.res)

add_custom_command(
    OUTPUT ${RESOURCE_OUTPUT}
    COMMAND rc /fo ${RESOURCE_OUTPUT} ${RESOURCE_FILE}
    DEPENDS ${RESOURCE_FILE}
)

add_custom_target(Resource ALL DEPENDS ${RESOURCE_OUTPUT})
include(FetchContent)

set(ENABLE_SKYRIM_SE ON CACHE BOOL " " FORCE)
set(ENABLE_SKYRIM_AE ON CACHE BOOL " " FORCE)
set(ENABLE_SKYRIM_VR ON CACHE BOOL " " FORCE)
set(BUILD_TESTS OFF CACHE BOOL " " FORCE)

add_subdirectory("$ENV{COMMONLIB_SSE_FOLDER}" ${CMAKE_CURRENT_BINARY_DIR}/_deps/clib-build)
get_target_property(COMMONLIB_SRC_DIR CommonLibSSE SOURCE_DIR)

find_path(CLIB_UTIL_INCLUDE_DIRS "ClibUtil/utils.hpp")

file(
  GLOB_RECURSE
  sources
  ${CMAKE_CURRENT_BINARY_DIR}/version.rc
  ${sources}
)

source_group(
  TREE ${CMAKE_CURRENT_SOURCE_DIR}
  FILES ${headers} ${sources}
)

set(BUILD_TESTS OFF)
include(${COMMONLIB_SRC_DIR}/cmake/CommonLibSSE.cmake)

add_commonlibsse_plugin(
  ${PROJECT_NAME}
  AUTHOR AUTHOR_NAME
  SOURCES ${headers} ${sources}
)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23) 
target_precompile_headers(${PROJECT_NAME} PRIVATE include/PCH.h)

target_include_directories(
	${PROJECT_NAME}
	PRIVATE
	${CLIB_UTIL_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends
)

#TODO: This must be adjusted for release

find_library(DIRECTXTK_LIBRARY NAMES DirectXTK PATHS ${CMAKE_BINARY_DIR}/debug/vcpkg_installed/x64-windows-static/lib)
target_link_libraries(${PROJECT_NAME} PRIVATE ${DIRECTXTK_LIBRARY})

set(wildlander_output false)
set(steam_owrt_output false)
set(steam_mods_output true)
automaticGameFolderOutput(wildlander_output, steam_owrt_output, steam_mods_output)
