find_package(AWS4)

if(TARGET AWS4::static)
  return()
endif()

set(AWS4_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/include)

include(ExternalProject)
include(AddIWNET)

if(NOT DEFINED AWS4_URL)
  set(AWS4_URL
      https://github.com/Softmotions/aws4/archive/refs/heads/master.zip)
endif()

set(BYPRODUCT "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}/libaws4-1.a")

set(CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
               -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR})

set(CMAKE_FIND_ROOT_PATH ${CMAKE_FIND_ROOT_PATH} ${CMAKE_INSTALL_PREFIX}
                         ${CMAKE_BINARY_DIR})

list(REMOVE_DUPLICATES CMAKE_FIND_ROOT_PATH)

set(SSUB "|")
foreach(
  extra
  ANDROID_ABI
  ANDROID_PLATFORM
  ARCHS
  CMAKE_C_COMPILER
  CMAKE_FIND_ROOT_PATH
  CMAKE_OSX_ARCHITECTURES
  CMAKE_TOOLCHAIN_FILE
  ENABLE_ARC
  ENABLE_BITCODE
  ENABLE_STRICT_TRY_COMPILE
  ENABLE_VISIBILITY
  PLATFORM
  TEST_TOOL_CMD)
  if(DEFINED ${extra})
    string(REPLACE ";" "${SSUB}" extra_sub "${${extra}}")
    list(APPEND CMAKE_ARGS "-D${extra}=${extra_sub}")
  endif()
endforeach()

message("AWS4 CMAKE_ARGS: ${CMAKE_ARGS}")

ExternalProject_Add(
  extern_aws4
  URL ${AWS4_URL}
  DOWNLOAD_NAME aws4.zip
  TIMEOUT 360
  PREFIX ${CMAKE_BINARY_DIR}
  BUILD_IN_SOURCE OFF
  UPDATE_COMMAND ""
  CMAKE_ARGS ${CMAKE_ARGS}
  LIST_SEPARATOR "${SSUB}"
  BUILD_BYPRODUCTS ${BYPRODUCT})

include(FindCURL)
if(NOT CURL_FOUND)
  message(FATAL_ERROR "Cannot find libcurl library")
endif()

add_library(AWS4::static STATIC IMPORTED GLOBAL)
set_target_properties(
  AWS4::static
  PROPERTIES IMPORTED_LINK_INTERFACE_LANGUAGES "C"
             IMPORTED_LOCATION ${BYPRODUCT}
             IMPORTED_LINK_INTERFACE_LIBRARIES
             "IWNET::static;${CURL_LIBRARIES}")

add_dependencies(AWS4::static extern_aws4)
