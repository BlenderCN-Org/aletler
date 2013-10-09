import os

EIGEN_PATH = "/usr/local/include/eigen-eigen-ffa86ffb5570/"

env = Environment(
    ENV = {'PATH' : os.environ['PATH'],
           },
    CXX = "clang++",
    CXXFLAGS = ["-std=c++11", 
    #                "-Weverything", 
                #"-Wno-c++98-compat",
                #"-pedantic",
            ],
    )

env.Append(CPPPATH = ['include'])

env.Library('lib/aletler-geometry',
            Glob('src/geometry/*.cpp'),
            CPPPATH = ['include/geometry'])
"""
env.Library('lib/aletler-sound',
            Glob('src/sound/*.cpp'),
            CXX = "g++", # because Eigen doesn't seem to like clang
            CXXFLAGS = "",
            CPPPATH = ['include/sound',
                       EIGEN_PATH])
"""

env.Program('bin/tests/geometry',
            Glob('tests/geometry/*.cpp'),
            LIBS = ['aletler-geometry'],
            LIBPATH = ['lib']
           )
