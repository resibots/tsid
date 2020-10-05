#----------------------------------------------------------------
# Generated CMake target import file for configuration "RELEASE".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "tsid::tsid" for configuration "RELEASE"
set_property(TARGET tsid::tsid APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(tsid::tsid PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libtsid.so.1.4.1"
  IMPORTED_SONAME_RELEASE "libtsid.so.1.4.1"
  )

list(APPEND _IMPORT_CHECK_TARGETS tsid::tsid )
list(APPEND _IMPORT_CHECK_FILES_FOR_tsid::tsid "${_IMPORT_PREFIX}/lib/libtsid.so.1.4.1" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
