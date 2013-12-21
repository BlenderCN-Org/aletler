# - Find sndfile
# Find the native sndfile includes and libraries
#
#  SNDFILE_INCLUDE_DIR - where to find sndfile.h, etc.
#  SNDFILE_LIBRARIES   - List of libraries when using libsndfile.
#  SNDFILE_FOUND       - True if libsndfile found.

IF(SNDFILE_INCLUDE_DIR)
    # Already in cache, be silent
    SET(SNDFILE_FIND_QUIETLY TRUE)
ENDIF(SNDFILE_INCLUDE_DIR)

find_path(SNDFILE_INCLUDE_DIR sndfile.h)

find_library(SNDFILE_LIBRARY NAMES sndfile sndfile-1)

# Handle the QUIETLY and REQUIRED arguments and set SNDFILE_FOUND to TRUE if
# all listed variables are TRUE.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SNDFILE DEFAULT_MSG
    SNDFILE_INCLUDE_DIR SNDFILE_LIBRARY)

IF(SNDFILE_FOUND)
  SET(SNDFILE_LIBRARIES ${SNDFILE_LIBRARY})
ELSE(SNDFILE_FOUND)
  SET(SNDFILE_LIBRARIES)
ENDIF(SNDFILE_FOUND)

mark_as_advanced(SNDFILE_INCLUDE_DIR SNDFILE_LIBRARY)

