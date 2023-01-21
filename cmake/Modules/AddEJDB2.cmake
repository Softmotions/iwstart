find_package(EJDB2)

if(TARGET EJDB2::static)
  return()
endif()

set(EJDB2_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/include)

include(ExternalProject)
include(AddIWNET)

if(NOT DEFINED EJDB2_URL)
  set(EJDB2_URL
      https://github.com/Softmotions/ejdb/archive/refs/heads/master.zip)
endif()

set(BYPRODUCT "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}/libejdb2-2.a")

set(CMAKE_ARGS
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}
    -DINSTALL_FLAT_SUBPROJECT_INCLUDES=ON
    -DBUILD_SHARED_LIBS=OFF
    -DBUILD_EXAMPLES=OFF)

set(CMAKE_FIND_ROOT_PATH ${CMAKE_FIND_ROOT_PATH} ${CMAKE_INSTALL_PREFIX}
                         ${CMAKE_BINARY_DIR})

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

message("EJDB CMAKE_ARGS: ${CMAKE_ARGS}")

ExternalProject_Add(
  extern_ejdb2
  URL ${EJDB2_URL}
  DOWNLOAD_NAME ejdb2.zip
  TIMEOUT 360
  PREFIX ${CMAKE_BINARY_DIR}
  BUILD_IN_SOURCE OFF
  # DOWNLOAD_EXTRACT_TIMESTAMP ON
  UPDATE_COMMAND ""
  CMAKE_ARGS ${CMAKE_ARGS}
  LIST_SEPARATOR "${SSUB}"
  BUILD_BYPRODUCTS ${BYPRODUCT})

add_library(EJDB2::static STATIC IMPORTED GLOBAL)
set_target_properties(
  EJDB2::static
  PROPERTIES IMPORTED_LINK_INTERFACE_LANGUAGES "C"
             IMPORTED_LOCATION
             ${BYPRODUCT}
             IMPORTED_LINK_INTERFACE_LIBRARIES "IWNET::static")

add_dependencies(EJDB2::static extern_ejdb2)
