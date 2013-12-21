disable_eigen_warnings()

add_snd_exe(bmbie-solve
        BMBIESolve.cpp
        ../fsm/BIESpec.cpp
        ../fsm/BIESolver.cpp
        ../fsm/BMBIESolver.cpp
        ../gauss_quadra/gauss_legendre.c
        ../fsm/dunavant.cpp
        ../utils/nano_timer.c
        LINK_LIBRARIES 
        ${TBB_LIBRARY} ${MKL_LIBS} rt
        OUT_DIR ${BIN_DIR})

