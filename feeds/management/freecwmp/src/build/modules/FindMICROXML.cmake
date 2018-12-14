# MICROXML_FOUND - true if library and headers were found
# MICROXML_INCLUDE_DIRS - include directories
# MICROXML_LIBRARIES - library directories

find_package(PkgConfig)
pkg_check_modules(PC_MICROXML QUIET microxml)

find_path(MICROXML_INCLUDE_DIR microxml.h
	HINTS ${PC_MICROXML_INCLUDEDIR} ${PC_MICROXML_INCLUDE_DIRS})

find_library(MICROXML_LIBRARY NAMES microxml libmicroxml
	HINTS ${PC_MICROXML_LIBDIR} ${PC_MICROXML_LIBRARY_DIRS})

set(MICROXML_LIBRARIES ${MICROXML_LIBRARY})
set(MICROXML_INCLUDE_DIRS ${MICROXML_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(MICROXML DEFAULT_MSG MICROXML_LIBRARY MICROXML_INCLUDE_DIR)

mark_as_advanced(MICROXML_INCLUDE_DIR MICROXML_LIBRARY)
