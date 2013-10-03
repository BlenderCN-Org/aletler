import os


env = Environment(
    ENV = {'PATH' : os.environ['PATH'],
           },
    CXX = "clang++",
    CXXFLAGS = ["-std=c++11", "-Weverything", "-pedantic"]
    )

env.Library('lib/aletler-geometry',
            Glob('src/geometry/*.cpp'),
            CPPPATH = ['include/geometry'])
