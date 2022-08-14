cmake_minimum_required(VERSION 3.18.0 FATAL_ERROR)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
                      "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")

set(DEB_CHANGELOG_REQUIRED ON)
set(DEB_CHANGELOG "${CMAKE_CURRENT_SOURCE_DIR}/Changelog")
unset(CHANGELOG_LAST_VERSION)
unset(CHANGELOG_LAST_MESSAGE)

include(DebChangelog)
include(GitRevision)

set(PROJECT_NAME "{project_name}")
set(PROJECT_VENDOR "{project_vendor}")
set(PROJECT_WEBSITE "{project_website}")
set(PROJECT_MAINTAINER "{project_author}")
set(PROJECT_DESCRIPTION_SUMMARY "{project_description}")
set(PROJECT_DESCRIPTION ${PROJECT_DESCRIPTION_SUMMARY})
set(PROJECT_VERSION_MAJOR ${CHANGELOG_LAST_VERSION_MAJOR})
set(PROJECT_VERSION_MINOR ${CHANGELOG_LAST_VERSION_MINOR})
set(PROJECT_VERSION_PATCH ${CHANGELOG_LAST_VERSION_PATCH})
set(PROJECT_VERSION
    ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})
set(${PROJECT_NAME}_VERSION ${PROJECT_VERSION})
set(${PROJECT_NAME}_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(${PROJECT_NAME}_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(${PROJECT_NAME}_VERSION_PATCH ${PROJECT_VERSION_PATCH})

project(
  ${PROJECT_NAME}
  VERSION ${PROJECT_VERSION}
  LANGUAGES C)

include(GNUInstallDirs)
include(ProjectUtils)

option(BUILD_TESTS "Build test cases" OFF)

if(BUILD_TESTS)
  include(CTest)
  enable_testing()
  add_definitions(-DIW_TESTS)
endif()

macro_ensure_out_of_source_build(
  "${CMAKE_PROJECT_NAME} requires an out of source build.")

add_subdirectory(src)
add_subdirectory(tools)
