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
    $<TARGET_FILE:hoststrliteral> -i "cmake_iowow_add" -e
    'static const char* cmake_iowow_add_path = \"cmake/Modules/AddIOWOW.cmake\"\;'
    ${CMAKE_SOURCE_DIR}/cmake/Modules/AddIOWOW.cmake >
    "${CMAKE_BINARY_DIR}/include/cmake_iowow_add.inc")

list(APPEND _TARGETS "${CMAKE_BINARY_DIR}/include/cmake_iwnet_add.inc")
add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}/include/cmake_iwnet_add.inc"
  DEPENDS hoststrliteral
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMAND cmake -E make_directory ${CMAKE_BINARY_DIR}/include
  COMMAND
    $<TARGET_FILE:hoststrliteral> -i "cmake_iwnet_add" -e
    'static const char* cmake_iwnet_add_path = \"cmake/Modules/AddIWNET.cmake\"\;'
    ${CMAKE_SOURCE_DIR}/cmake/Modules/AddIWNET.cmake >
    "${CMAKE_BINARY_DIR}/include/cmake_iwnet_add.inc")

list(APPEND _TARGETS "${CMAKE_BINARY_DIR}/include/cmake_ejdb2_add.inc")
add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}/include/cmake_ejdb2_add.inc"
  DEPENDS hoststrliteral
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMAND cmake -E make_directory ${CMAKE_BINARY_DIR}/include
  COMMAND
    $<TARGET_FILE:hoststrliteral> -i "cmake_ejdb2_add" -e
    'static const char* cmake_ejdb2_add_path = \"cmake/Modules/AddEJDB2.cmake\"\;'
    ${CMAKE_SOURCE_DIR}/cmake/Modules/AddEJDB2.cmake >
    "${CMAKE_BINARY_DIR}/include/cmake_ejdb2_add.inc")

list(APPEND _TARGETS "${CMAKE_BINARY_DIR}/include/cmake_aws4_add.inc")
add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}/include/cmake_aws4_add.inc"
  DEPENDS hoststrliteral
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMAND cmake -E make_directory ${CMAKE_BINARY_DIR}/include
  COMMAND
    $<TARGET_FILE:hoststrliteral> -i "cmake_aws4_add" -e
    'static const char* cmake_aws4_add_path = \"cmake/Modules/AddAWS4.cmake\"\;'
    ${CMAKE_SOURCE_DIR}/cmake/Modules/AddAWS4.cmake >
    "${CMAKE_BINARY_DIR}/include/cmake_aws4_add.inc")

list(APPEND _TARGETS "${CMAKE_BINARY_DIR}/include/cmake_iowow_find.inc")
add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}/include/cmake_iowow_find.inc"
  DEPENDS hoststrliteral
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMAND cmake -E make_directory ${CMAKE_BINARY_DIR}/include
  COMMAND
    $<TARGET_FILE:hoststrliteral> -i "cmake_iowow_find" -e
    'static const char* cmake_iowow_find_path = \"cmake/Modules/FindIOWOW.cmake\"\;'
    ${CMAKE_SOURCE_DIR}/cmake/Modules/FindIOWOW.cmake >
    "${CMAKE_BINARY_DIR}/include/cmake_iowow_find.inc")

list(APPEND _TARGETS "${CMAKE_BINARY_DIR}/include/cmake_iwnet_find.inc")
add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}/include/cmake_iwnet_find.inc"
  DEPENDS hoststrliteral
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMAND cmake -E make_directory ${CMAKE_BINARY_DIR}/include
  COMMAND
    $<TARGET_FILE:hoststrliteral> -i "cmake_iwnet_find" -e
    'static const char* cmake_iwnet_find_path = \"cmake/Modules/FindIWNET.cmake\"\;'
    ${CMAKE_SOURCE_DIR}/cmake/Modules/FindIWNET.cmake >
    "${CMAKE_BINARY_DIR}/include/cmake_iwnet_find.inc")

list(APPEND _TARGETS "${CMAKE_BINARY_DIR}/include/cmake_ejdb2_find.inc")
add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}/include/cmake_ejdb2_find.inc"
  DEPENDS hoststrliteral
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMAND cmake -E make_directory ${CMAKE_BINARY_DIR}/include
  COMMAND
    $<TARGET_FILE:hoststrliteral> -i "cmake_ejdb2_find" -e
    'static const char* cmake_ejdb2_find_path = \"cmake/Modules/FindEJDB2.cmake\"\;'
    ${CMAKE_SOURCE_DIR}/cmake/Modules/FindEJDB2.cmake >
    "${CMAKE_BINARY_DIR}/include/cmake_ejdb2_find.inc")

list(APPEND _TARGETS "${CMAKE_BINARY_DIR}/include/cmake_aws4_find.inc")
add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}/include/cmake_aws4_find.inc"
  DEPENDS hoststrliteral
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMAND cmake -E make_directory ${CMAKE_BINARY_DIR}/include
  COMMAND
    $<TARGET_FILE:hoststrliteral> -i "cmake_aws4_find" -e
    'static const char* cmake_aws4_find_path = \"cmake/Modules/FindAWS4.cmake\"\;'
    ${CMAKE_SOURCE_DIR}/cmake/Modules/FindAWS4.cmake >
    "${CMAKE_BINARY_DIR}/include/cmake_aws4_find.inc")

list(APPEND _TARGETS "${CMAKE_BINARY_DIR}/include/cmake_deb_changelog.inc")
add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}/include/cmake_deb_changelog.inc"
  DEPENDS hoststrliteral
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMAND cmake -E make_directory ${CMAKE_BINARY_DIR}/include
  COMMAND
    $<TARGET_FILE:hoststrliteral> -i "cmake_deb_changelog" -e
    'static const char* cmake_deb_changelog_path = \"cmake/Modules/DebChangelog.cmake\"\;'
    ${CMAKE_SOURCE_DIR}/cmake/Modules/DebChangelog.cmake >
    "${CMAKE_BINARY_DIR}/include/cmake_deb_changelog.inc")

list(APPEND _TARGETS "${CMAKE_BINARY_DIR}/include/cmake_git_revision.inc")
add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}/include/cmake_git_revision.inc"
  DEPENDS hoststrliteral
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMAND cmake -E make_directory ${CMAKE_BINARY_DIR}/include
  COMMAND
    $<TARGET_FILE:hoststrliteral> -i "cmake_git_revision" -e
    'static const char* cmake_git_revision_path = \"cmake/Modules/GitRevision.cmake\"\;'
    ${CMAKE_SOURCE_DIR}/cmake/Modules/GitRevision.cmake >
    "${CMAKE_BINARY_DIR}/include/cmake_git_revision.inc")

list(APPEND _TARGETS "${CMAKE_BINARY_DIR}/include/cmake_project_utils.inc")
add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}/include/cmake_project_utils.inc"
  DEPENDS hoststrliteral
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMAND cmake -E make_directory ${CMAKE_BINARY_DIR}/include
  COMMAND
    $<TARGET_FILE:hoststrliteral> -i "cmake_project_utils" -e
    'static const char* cmake_project_utils_path = \"cmake/Modules/ProjectUtils.cmake\"\;'
    ${CMAKE_SOURCE_DIR}/cmake/Modules/ProjectUtils.cmake >
    "${CMAKE_BINARY_DIR}/include/cmake_project_utils.inc")

list(APPEND _TARGETS "${CMAKE_BINARY_DIR}/include/cmake_lists.inc")
add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}/include/cmake_lists.inc"
  DEPENDS hoststrliteral
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMAND cmake -E make_directory ${CMAKE_BINARY_DIR}/include
  COMMAND
    $<TARGET_FILE:hoststrliteral> -i "cmake_lists" -e
    'static const char* cmake_lists_path = \"CMakeLists.txt\"\;'
    ${CMAKE_SOURCE_DIR}/templates/CMakeLists.cmake >
    "${CMAKE_BINARY_DIR}/include/cmake_lists.inc")

list(APPEND _TARGETS "${CMAKE_BINARY_DIR}/include/src_cmake_lists.inc")
add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}/include/src_cmake_lists.inc"
  DEPENDS hoststrliteral
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMAND cmake -E make_directory ${CMAKE_BINARY_DIR}/include
  COMMAND
    $<TARGET_FILE:hoststrliteral> -i "src_cmake_lists" -e
    'static const char* src_cmake_lists_path = \"src/CMakeLists.txt\"\;'
    ${CMAKE_SOURCE_DIR}/templates/SrcCMakeLists.cmake >
    "${CMAKE_BINARY_DIR}/include/src_cmake_lists.inc")

list(APPEND _TARGETS "${CMAKE_BINARY_DIR}/include/app_c.inc")
add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}/include/app_c.inc"
  DEPENDS hoststrliteral
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMAND cmake -E make_directory ${CMAKE_BINARY_DIR}/include
  COMMAND
    $<TARGET_FILE:hoststrliteral> -i "app_c" -e
    'static const char* app_c_path = \"src/{project_artifact}.c\"\;'
    ${CMAKE_SOURCE_DIR}/templates/app.c >
    "${CMAKE_BINARY_DIR}/include/app_c.inc")

list(APPEND _TARGETS "${CMAKE_BINARY_DIR}/include/app_h.inc")
add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}/include/app_h.inc"
  DEPENDS hoststrliteral
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMAND cmake -E make_directory ${CMAKE_BINARY_DIR}/include
  COMMAND
    $<TARGET_FILE:hoststrliteral> -i "app_h" -e
    'static const char* app_h_path = \"src/{project_artifact}.h\"\;'
    ${CMAKE_SOURCE_DIR}/templates/app.h >
    "${CMAKE_BINARY_DIR}/include/app_h.inc")

list(APPEND _TARGETS "${CMAKE_BINARY_DIR}/include/config_h.inc")
add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}/include/config_h.inc"
  DEPENDS hoststrliteral
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMAND cmake -E make_directory ${CMAKE_BINARY_DIR}/include
  COMMAND
    $<TARGET_FILE:hoststrliteral> -i "config_h" -e
    'static const char* config_h_path = \"src/config.h.in\"\;'
    ${CMAKE_SOURCE_DIR}/src/config.h.in >
    "${CMAKE_BINARY_DIR}/include/config_h.inc")

list(APPEND _TARGETS "${CMAKE_BINARY_DIR}/include/readme_md.inc")
add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}/include/readme_md.inc"
  DEPENDS hoststrliteral
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMAND cmake -E make_directory ${CMAKE_BINARY_DIR}/include
  COMMAND
    $<TARGET_FILE:hoststrliteral> -i "readme_md" -e
    'static const char* readme_md_path = \"README.md\"\;'
    ${CMAKE_SOURCE_DIR}/templates/README.md >
    "${CMAKE_BINARY_DIR}/include/readme_md.inc")

list(APPEND _TARGETS "${CMAKE_BINARY_DIR}/include/test1.inc")
add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}/include/test1.inc"
  DEPENDS hoststrliteral
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMAND cmake -E make_directory ${CMAKE_BINARY_DIR}/include
  COMMAND
    $<TARGET_FILE:hoststrliteral> -i "test1" -e
    'static const char* test1_path = \"src/tests/test1.c\"\;'
    ${CMAKE_SOURCE_DIR}/templates/test1.c >
    "${CMAKE_BINARY_DIR}/include/test1.inc")

list(APPEND _TARGETS "${CMAKE_BINARY_DIR}/include/tests_cmake_lists.inc")
add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}/include/tests_cmake_lists.inc"
  DEPENDS hoststrliteral
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMAND cmake -E make_directory ${CMAKE_BINARY_DIR}/include
  COMMAND
    $<TARGET_FILE:hoststrliteral> -i "tests_cmake_lists" -e
    'static const char* tests_cmake_lists_path = \"src/tests/CMakeLists.txt\"\;'
    ${CMAKE_SOURCE_DIR}/templates/TestsCMakeLists.cmake >
    "${CMAKE_BINARY_DIR}/include/tests_cmake_lists.inc")

list(APPEND _TARGETS "${CMAKE_BINARY_DIR}/include/tools_cmake_lists.inc")
add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}/include/tools_cmake_lists.inc"
  DEPENDS hoststrliteral
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMAND cmake -E make_directory ${CMAKE_BINARY_DIR}/include
  COMMAND
    $<TARGET_FILE:hoststrliteral> -i "tools_cmake_lists" -e
    'static const char* tools_cmake_lists_path = \"tools/CMakeLists.txt\"\;'
    ${CMAKE_SOURCE_DIR}/templates/ToolsCMakeLists.cmake >
    "${CMAKE_BINARY_DIR}/include/tools_cmake_lists.inc")

list(APPEND _TARGETS "${CMAKE_BINARY_DIR}/include/tools_strliteral.inc")
add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}/include/tools_strliteral.inc"
  DEPENDS hoststrliteral
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMAND cmake -E make_directory ${CMAKE_BINARY_DIR}/include
  COMMAND
    $<TARGET_FILE:hoststrliteral> -i "tools_strliteral" -e
    'static const char* tools_strliteral_path = \"tools/strliteral.c\"\;'
    ${CMAKE_SOURCE_DIR}/tools/strliteral.c >
    "${CMAKE_BINARY_DIR}/include/tools_strliteral.inc")

list(APPEND _TARGETS "${CMAKE_BINARY_DIR}/include/app_run.inc")
add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}/include/app_run.inc"
  DEPENDS hoststrliteral
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMAND cmake -E make_directory ${CMAKE_BINARY_DIR}/include
  COMMAND
    $<TARGET_FILE:hoststrliteral> -i "app_run" -e
    'static const char* app_run_path = \"src/{project_artifact}_run.c\"\;'
    ${CMAKE_SOURCE_DIR}/templates/app_run.c >
    "${CMAKE_BINARY_DIR}/include/app_run.inc")

list(APPEND _TARGETS "${CMAKE_BINARY_DIR}/include/changelog.inc")
add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}/include/changelog.inc"
  DEPENDS hoststrliteral
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMAND cmake -E make_directory ${CMAKE_BINARY_DIR}/include
  COMMAND
    $<TARGET_FILE:hoststrliteral> -i "changelog" -e
    'static const char* changelog_path = \"Changelog\"\;'
    ${CMAKE_SOURCE_DIR}/templates/Changelog >
    "${CMAKE_BINARY_DIR}/include/changelog.inc")

list(APPEND _TARGETS "${CMAKE_BINARY_DIR}/include/license.inc")
add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}/include/license.inc"
  DEPENDS hoststrliteral
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMAND cmake -E make_directory ${CMAKE_BINARY_DIR}/include
  COMMAND
    $<TARGET_FILE:hoststrliteral> -i "license" -e
    'static const char* license_path = \"LICENSE\"\;'
    ${CMAKE_SOURCE_DIR}/templates/LICENSE >
    "${CMAKE_BINARY_DIR}/include/license.inc")

list(APPEND _TARGETS "${CMAKE_BINARY_DIR}/include/gitignore.inc")
add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}/include/gitignore.inc"
  DEPENDS hoststrliteral
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMAND cmake -E make_directory ${CMAKE_BINARY_DIR}/include
  COMMAND
    $<TARGET_FILE:hoststrliteral> -i "gitignore" -e
    'static const char* gitignore_path = \".gitignore\"\;'
    ${CMAKE_SOURCE_DIR}/.gitignore >
    "${CMAKE_BINARY_DIR}/include/gitignore.inc")

list(APPEND _TARGETS "${CMAKE_BINARY_DIR}/include/uncrustify.inc")
add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}/include/uncrustify.inc"
  DEPENDS hoststrliteral
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMAND cmake -E make_directory ${CMAKE_BINARY_DIR}/include
  COMMAND
    $<TARGET_FILE:hoststrliteral> -i "uncrustify" -e
    'static const char* uncrustify_path = \"uncrustify.cfg\"\;'
    ${CMAKE_SOURCE_DIR}/uncrustify.cfg >
    "${CMAKE_BINARY_DIR}/include/uncrustify.inc")

list(APPEND _TARGETS "${CMAKE_BINARY_DIR}/include/lvimrc.inc")
add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}/include/lvimrc.inc"
  DEPENDS hoststrliteral
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMAND cmake -E make_directory ${CMAKE_BINARY_DIR}/include
  COMMAND
    $<TARGET_FILE:hoststrliteral> -i "lvimrc" -e
    'static const char* lvimrc_path = \".lvimrc\"\;'
    ${CMAKE_SOURCE_DIR}/.lvimrc > "${CMAKE_BINARY_DIR}/include/lvimrc.inc")

add_custom_target(generated DEPENDS ${_TARGETS})
