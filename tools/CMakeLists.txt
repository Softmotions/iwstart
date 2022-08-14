cmake_minimum_required(VERSION 3.18.0 FATAL_ERROR)
project(tools LANGUAGES C)

set(_TARGETS)
set(_TOOLS_LIST strliteral)

if(CMAKE_CROSSCOMPILING)
  file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/host_tools)
  file(TO_NATIVE_PATH ${CMAKE_COMMAND} native_cmake_command)
  file(TO_NATIVE_PATH ${CMAKE_CURRENT_SOURCE_DIR}
       native_cmake_current_source_dir)

  execute_process(
    COMMAND_ECHO STDOUT
    COMMAND
      sh -c
      "CC= ${native_cmake_command} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} ${native_cmake_current_source_dir}"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/host_tools)

  add_custom_target(
    host_tools
    COMMAND ${CMAKE_COMMAND} --build . --target host_tools --config $<CONFIG>
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/host_tools)

  include(${CMAKE_CURRENT_BINARY_DIR}/host_tools/host_tools.cmake)

  foreach(tgt IN ITEMS ${_TOOLS_LIST})
    add_custom_target(
      host${tgt}ninja
      COMMAND echo ''
      DEPENDS host_tools
      BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/host_tools/${tgt})
    add_dependencies(host${tgt} host${tgt}ninja)
  endforeach()

else(!CMAKE_CROSSCOMPILING)
  add_custom_target(host_tools DEPENDS ${_TOOLS_LIST})
endif(CMAKE_CROSSCOMPILING)

add_executable(strliteral strliteral.c)

if(NOT CMAKE_CROSSCOMPILING)
  foreach(tgt IN ITEMS ${_TOOLS_LIST})
    add_executable(host${tgt} ALIAS ${tgt})
  endforeach()
  export(
    TARGETS ${_TOOLS_LIST}
    NAMESPACE host
    FILE host_tools.cmake)
endif()

list(APPEND _TARGETS "${CMAKE_BINARY_DIR}/include/cmake_iowow_add.inc")
add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}/include/cmake_iowow_add.inc"
  DEPENDS hoststrliteral
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMAND cmake -E make_directory ${CMAKE_BINARY_DIR}/include
  COMMAND
    $<TARGET_FILE:hoststrliteral> -i "cmake_iowow_add"
    ${CMAKE_SOURCE_DIR}/cmake/Modules/AddIOWOW.cmake >
    "${CMAKE_BINARY_DIR}/include/cmake_iowow_add.inc")

list(APPEND _TARGETS "${CMAKE_BINARY_DIR}/include/cmake_iwnet_add.inc")
add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}/include/cmake_iwnet_add.inc"
  DEPENDS hoststrliteral
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMAND cmake -E make_directory ${CMAKE_BINARY_DIR}/include
  COMMAND
    $<TARGET_FILE:hoststrliteral> -i "cmake_iwnet_add"
    ${CMAKE_SOURCE_DIR}/cmake/Modules/AddIWNET.cmake >
    "${CMAKE_BINARY_DIR}/include/cmake_iwnet_add.inc")

list(APPEND _TARGETS "${CMAKE_BINARY_DIR}/include/cmake_ejdb2_add.inc")
add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}/include/cmake_ejdb2_add.inc"
  DEPENDS hoststrliteral
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMAND cmake -E make_directory ${CMAKE_BINARY_DIR}/include
  COMMAND
    $<TARGET_FILE:hoststrliteral> -i "cmake_ejdb2_add"
    ${CMAKE_SOURCE_DIR}/cmake/Modules/AddEJDB2.cmake >
    "${CMAKE_BINARY_DIR}/include/cmake_ejdb2_add.inc")

list(APPEND _TARGETS "${CMAKE_BINARY_DIR}/include/cmake_lists.inc")
add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}/include/cmake_lists.inc"
  DEPENDS hoststrliteral
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMAND cmake -E make_directory ${CMAKE_BINARY_DIR}/include
  COMMAND
    $<TARGET_FILE:hoststrliteral> -i "cmake_lists"
    ${CMAKE_SOURCE_DIR}/templates/CMakeLists.cmake >
    "${CMAKE_BINARY_DIR}/include/cmake_lists.inc")

list(APPEND _TARGETS "${CMAKE_BINARY_DIR}/include/src_cmake_lists.inc")
add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}/include/src_cmake_lists.inc"
  DEPENDS hoststrliteral
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMAND cmake -E make_directory ${CMAKE_BINARY_DIR}/include
  COMMAND
    $<TARGET_FILE:hoststrliteral> -i "src_cmake_lists"
    ${CMAKE_SOURCE_DIR}/templates/SrcCMakeLists.cmake >
    "${CMAKE_BINARY_DIR}/include/src_cmake_lists.inc")

list(APPEND _TARGETS "${CMAKE_BINARY_DIR}/include/app_c.inc")
add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}/include/app_c.inc"
  DEPENDS hoststrliteral
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMAND cmake -E make_directory ${CMAKE_BINARY_DIR}/include
  COMMAND
    $<TARGET_FILE:hoststrliteral> -i "app_c"
    ${CMAKE_SOURCE_DIR}/templates/app.c >
    "${CMAKE_BINARY_DIR}/include/app_c.inc")

list(APPEND _TARGETS "${CMAKE_BINARY_DIR}/include/app_h.inc")
add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}/include/app_h.inc"
  DEPENDS hoststrliteral
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMAND cmake -E make_directory ${CMAKE_BINARY_DIR}/include
  COMMAND
    $<TARGET_FILE:hoststrliteral> -i "app_h"
    ${CMAKE_SOURCE_DIR}/templates/app.h >
    "${CMAKE_BINARY_DIR}/include/app_h.inc")

list(APPEND _TARGETS "${CMAKE_BINARY_DIR}/include/app_json.inc")
add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}/include/app_json.inc"
  DEPENDS hoststrliteral
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMAND cmake -E make_directory ${CMAKE_BINARY_DIR}/include
  COMMAND
    $<TARGET_FILE:hoststrliteral> -i "app_json"
    ${CMAKE_SOURCE_DIR}/templates/app.json >
    "${CMAKE_BINARY_DIR}/include/app_json.inc")

list(APPEND _TARGETS "${CMAKE_BINARY_DIR}/include/readme_md.inc")
add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}/include/readme_md.inc"
  DEPENDS hoststrliteral
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMAND cmake -E make_directory ${CMAKE_BINARY_DIR}/include
  COMMAND
    $<TARGET_FILE:hoststrliteral> -i "readme_md"
    ${CMAKE_SOURCE_DIR}/templates/README.md >
    "${CMAKE_BINARY_DIR}/include/readme_md.inc")

list(APPEND _TARGETS "${CMAKE_BINARY_DIR}/include/test1.inc")
add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}/include/test1.inc"
  DEPENDS hoststrliteral
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMAND cmake -E make_directory ${CMAKE_BINARY_DIR}/include
  COMMAND
    $<TARGET_FILE:hoststrliteral> -i "test1"
    ${CMAKE_SOURCE_DIR}/templates/test1.c >
    "${CMAKE_BINARY_DIR}/include/test1.inc")

list(APPEND _TARGETS "${CMAKE_BINARY_DIR}/include/tests_cmake_lists.inc")
add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}/include/tests_cmake_lists.inc"
  DEPENDS hoststrliteral
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMAND cmake -E make_directory ${CMAKE_BINARY_DIR}/include
  COMMAND
    $<TARGET_FILE:hoststrliteral> -i "tests_cmake_lists"
    ${CMAKE_SOURCE_DIR}/templates/TestsCMakeLists.cmake >
    "${CMAKE_BINARY_DIR}/include/tests_cmake_lists.inc")

list(APPEND _TARGETS "${CMAKE_BINARY_DIR}/include/app_run.inc")
add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}/include/app_run.inc"
  DEPENDS hoststrliteral
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMAND cmake -E make_directory ${CMAKE_BINARY_DIR}/include
  COMMAND
    $<TARGET_FILE:hoststrliteral> -i "app_run"
    ${CMAKE_SOURCE_DIR}/templates/app_run.c >
    "${CMAKE_BINARY_DIR}/include/app_run.inc")

add_custom_target(generated DEPENDS ${_TARGETS})
