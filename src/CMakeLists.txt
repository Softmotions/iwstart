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
#include(AddIWNET)
#include(AddEJDB2)

set(LINK_LIBS IOWOW::static)
#set(LINK_LIBS IWNET::static)
#set(LINK_LIBS EJDB2::static)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_BINARY_DIR}/include)
add_definitions(-D_LARGEFILE64_SOURCE)

file(GLOB ALL_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.c)

add_executable(iwstart ${ALL_SRC})
target_link_libraries(iwstart ${LINK_LIBS})
set_target_properties(iwstart PROPERTIES COMPILE_FLAGS "-DIW_EXEC")

add_dependencies(iwstart generated)

if(BUILD_TESTS)
  add_library(iwstart_s ${ALL_SRC})
  target_link_libraries(iwstart_s PUBLIC ${LINK_LIBS})
  add_subdirectory(tests)
endif()
