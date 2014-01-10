#!/usr/bin/python2.7

from os import listdir
from os.path import isfile, join
import subprocess

bemin = "/Users/phaedon/github/aletler/meshes/geomsim3/fastbem/"
bemout = "/Users/phaedon/github/aletler/meshes/geomsim3/bemout/"
beminfiles = [ f for f in listdir(bemin) if isfile(join(bemin,f)) ]

bemoutfiles = [ "out"+f.split('.')[0]+".dat" for f in beminfiles ]

bmbie = "/Users/phaedon/Library/Developer/Xcode/DerivedData/aletler-acjiukzbnxmbbvhdctshyhuwjqhx/Build/Products/Release/bmbiesolve"

for i in range(len(beminfiles)):
    print "running file " + str(i)
    p = subprocess.Popen([bmbie, bemin + beminfiles[i], bemout + bemoutfiles[i]])

    
