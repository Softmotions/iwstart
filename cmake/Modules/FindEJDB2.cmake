mark_as_advanced(EJDB2_INCLUDE_DIRS EJDB2_STATIC_LIB)

find_path(EJDB2_INCLUDE_DIRS NAMES ejdb2/ejdb2.h)

find_library(EJDB2_STATIC_LIB NAMES libejdb2-1.a)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(EJDB2 DEFAULT_MSG EJDB2_INCLUDE_DIRS
                                  EJDB2_STATIC_LIB)
if(EJDB2_FOUND)
  add_library(EJDB2::static STATIC IMPORTED GLOBAL)
  set_target_properties(
    EJDB2::static
    PROPERTIES IMPORTED_LINK_INTERFACE_LANGUAGES "C"
               IMPORTED_LOCATION
               "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}/libejdb2-1.a"
               IMPORTED_LINK_INTERFACE_LIBRARIES "IWNET::static")
endif()
