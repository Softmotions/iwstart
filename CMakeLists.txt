cmake_minimum_required(VERSION 3.18.0 FATAL_ERROR)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
                      "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")

set(PROJECT_NAME "iwstart")
project(
  ${PROJECT_NAME}
  VERSION 1.0.0
  LANGUAGES C)

option(BUILD_TESTS "iwstart")

include(GitRevision)
include(GNUInstallDirs)
include(ProjectUtils)

if(BUILD_TESTS)
  include(CTest)
  enable_testing()
  add_definitions(-DIW_TESTS)
endif()

macro_ensure_out_of_source_build(
  "${CMAKE_PROJECT_NAME} requires an out of source build.")

add_subdirectory(src)
add_subdirectory(tools)
