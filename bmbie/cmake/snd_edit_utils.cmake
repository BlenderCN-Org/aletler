# Macros to build the targets
include( CMakeParseArguments )

# Usage:
# 
# add_snd_exe( name source1 [source2 ...]
#              [LINK_LIBRARIES lib1 ...]
#              [INCLUDE_DIRS dir1 ...]
#              [OUT_DIR "output dir"] )
function(add_snd_exe name)
    cmake_parse_arguments(_exe "" "OUT_DIR" "LINK_LIBRARIES;INCLUDE_DIRS" ${ARGN})
    set(_exe_srcs ${_exe_UNPARSED_ARGUMENTS})
    message(STATUS "--- add a executable target ${name} ---")

    add_executable(${name} ${_exe_srcs})
    if (_exe_OUT_DIR)
        set_target_properties(${name} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${_exe_OUT_DIR})
    endif()

    if (_exe_LINK_LIBRARIES)
        target_link_libraries(${name} ${_exe_LINK_LIBRARIES})
    endif()

    if (_exe_INCLUDE_DIRS)
        include_directories(${_exe_INCLUDE_DIRS})
    endif()
endfunction()
