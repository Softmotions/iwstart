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

include({project_base_lib_cmake})
set(LINK_LIBS {project_base_lib})

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_BINARY_DIR}/include)
add_definitions(-D_LARGEFILE_SOURCE)

file(GLOB ALL_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.c)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
               ${CMAKE_BINARY_DIR}/include/config.h)

add_executable({project_artifact} ${ALL_SRC})
target_link_libraries({project_artifact} ${LINK_LIBS})
set_target_properties({project_artifact} PROPERTIES COMPILE_FLAGS "-DIW_EXEC")

add_dependencies({project_artifact} generated)

if(BUILD_TESTS)
  add_library({project_artifact}_s ${ALL_SRC})
  target_link_libraries({project_artifact}_s PUBLIC ${LINK_LIBS})
  add_subdirectory(tests)
endif()
