import os

env = Environment(
    ENV = os.environ,

#    CXX = "clang++",
#    CXXFLAGS = ["-Weverything", 
#                "-Wno-c++98-compat",
#                "-pedantic",
#          ],
    LIBPATH = ['lib']
    )

env.Append(CPPPATH = ['include',
                      'include/geometry',
                      '/usr/local/include/eigen-eigen-ffa86ffb5570/',
                  ]
       )

env.Library('lib/aletler-geometry',
            Glob('src/geometry/*.cpp'),
            )

env.Library('lib/aletler-sound',
           Glob('src/sound/*.cpp'),
            LIBS = ['sndfile'],
            LIBPATH = ['/usr/local/lib'],
            CXXFLAGS = "",
            CPPPATH = ['include/sound',
                       '/opt/local/include',
                       '/usr/local/include/eigen-eigen-ffa86ffb5570/',
                   ]
)


env.Program('bin/tests/geometry',
            Glob('tests/geometry/*.cpp'),
            LIBS = ['aletler-geometry'],
           )

env.Append(LIBPATH = ['lib', '/usr/local/lib/'])

env.Program('bin/tests/sound',
            Glob('tests/sound/*.cpp'),
            LIBS = ['aletler-sound', 
                    'sndfile',
                ],

            CPPPATH = ['/opt/local/include',
                       'include',
                       '/usr/local/include/eigen-eigen-ffa86ffb5570/'],
            CXXFLAGS = "",
       )
