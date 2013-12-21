# Locate the KNITRO quadratic optimization library
#
# Define the following variables:
#    KNITRO_FOUND - Found the KNITRO library
#    KNITRO_INCLUDE_DIRS - Include directories
#
# Accept the following variables as input:
#    KNITRO_ROOT - (as CMake or env. variable)
#                 The root directory of the KNITRO installation

find_path(KNITRO_INCLUDE_DIR knitro.h
    HINTS
        $ENV{KNITRO_ROOT}/include
        ${KNITRO_ROOT}/include)
mark_as_advanced(KNITRO_INCLUDE_DIR)

find_library(KNITRO_LIBRARY NAMES knitro800
    HINTS
        $ENV{KNITRO_ROOT}/lib
        ${KNITRO_ROOT}/lib)

mark_as_advanced(KNITRO_LIBRARY)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(KNITRO DEFAULT_MSG KNITRO_LIBRARY KNITRO_INCLUDE_DIR)

if (KNITRO_FOUND)
    set(KNITRO_INCLUDE_DIRS ${KNITRO_INCLUDE_DIR})
    set(KNITRO_LIBRARIES ${KNITRO_LIBRARY})
endif (KNITRO_FOUND)


