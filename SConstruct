import os



env = Environment(
    ENV = {'PATH' : os.environ['PATH'],
           },
    CXX = "clang++",
    CXXFLAGS = ["-std=c++11", 
                "-Weverything", 
                "-Wno-c++98-compat",
                "-pedantic",]
    )

env.Append(CPPPATH = ['include'])

SConscript(dirs='tests', exports='env')

env.Library('lib/aletler-geometry',
            Glob('src/geometry/*.cpp'),
            CPPPATH = ['include/geometry'])
