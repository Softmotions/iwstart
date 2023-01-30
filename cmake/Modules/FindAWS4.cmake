mark_as_advanced(AWS4_INCLUDE_DIRS AWS4_STATIC_LIB)

find_path(AWS4_INCLUDE_DIRS NAMES aws4/aws4.h)
find_library(AWS4_STATIC_LIB NAMES libaws4-1.a)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(AWS4 DEFAULT_MSG AWS4_INCLUDE_DIRS
                                  AWS4_STATIC_LIB)

if(AWS4_FOUND)
  include(FindCURL)
  if(NOT CURL_FOUND)
    message(FATAL_ERROR "Cannot find libcurl library")
  endif()

  add_library(AWS4::static STATIC IMPORTED GLOBAL)
  set_target_properties(
    AWS4::static
    PROPERTIES IMPORTED_LINK_INTERFACE_LANGUAGES "C"
               IMPORTED_LOCATION ${AWS4_STATIC_LIB}
               IMPORTED_LINK_INTERFACE_LIBRARIES
               "IWNET::static;${CURL_LIBRARIES}")
endif()
