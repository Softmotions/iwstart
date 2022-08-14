if(NOT CMAKE_BUILD_TYPE)
  message(
    FATAL_ERROR
      "Please specify the build type -DCMAKE_BUILD_TYPE=Debug|Release|RelWithDebInfo"
  )
endif()

set(CMAKE_C_FLAGS
    "${CMAKE_C_FLAGS} \
  -std=gnu11 \
  -fPIC \
  -Wall \
  -Wextra \
  -Wfatal-errors \
  -Wno-implicit-fallthrough \
  -Wno-missing-braces \
  -Wno-missing-field-initializers \
  -Wno-sign-compare \
  -Wno-unknown-pragmas \
  -Wno-unused-function \
  -Wno-unused-parameter")

set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_C_FLAGS_DEBUG
    "-O0 -g -ggdb \
    -Werror \
    -Wno-unused-variable \
    -DDEBUG -D_DEBUG -UNDEBUG")

set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-Wl,-s")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELEASE} -g")
set(CMAKE_C_FLAGS_RELEASEWITHDEBINFO ${CMAKE_C_FLAGS_RELWITHDEBINFO})

find_package(Threads REQUIRED CMAKE_THREAD_PREFER_PTHREAD)

include(AddIOWOW)
# include(AddIWNET) include(AddEJDB2)

set(LINK_LIBS IOWOW::static)
# set(LINK_LIBS IWNET::static) set(LINK_LIBS EJDB2::static)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_BINARY_DIR}/include)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  # Needed by Linux in order to use nftw() but fail to build on FreeBSD due to
  # __BSD_VISIBLE define state.
  add_definitions(-D_XOPEN_SOURCE=700)
endif()
add_definitions(-D_DEFAULT_SOURCE)
add_definitions(-D_LARGEFILE_SOURCE)
add_definitions(-D_FILE_OFFSET_BITS=64)
if(APPLE)
  add_definitions(-D_DARWIN_C_SOURCE)
endif(APPLE)

file(GLOB ALL_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.c)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
               ${CMAKE_BINARY_DIR}/include/config.h)

add_executable(iwstart ${ALL_SRC})
target_link_libraries(iwstart ${LINK_LIBS})
set_target_properties(iwstart PROPERTIES COMPILE_FLAGS "-DIW_EXEC")

add_dependencies(iwstart generated)

if(BUILD_TESTS)
  add_library(iwstart_s ${ALL_SRC})
  target_link_libraries(iwstart_s PUBLIC ${LINK_LIBS})
  add_subdirectory(tests)
endif()
