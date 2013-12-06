#disable_eigen_warnings()

# -----------------------------------------------------------------------------
# mesh_simp
add_snd_exe(mesh_simp mesh_simp.cpp 
           ../remesh/MeshAttrSimplifier.hpp
            LINK_LIBRARIES ${Boost_SYSTEM_LIBRARY}
                           ${Boost_FILESYSTEM_LIBRARY}
                           ${Boost_PROGRAM_OPTIONS_LIBRARY}
                           ${CMAKE_THREAD_LIBS_INIT}
                           ${MKL_LIBS}
            OUT_DIR ${BIN_DIR})

