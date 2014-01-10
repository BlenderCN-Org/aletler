#!/usr/bin/python

import os, sys
import glob


SCRIPT_FILENAME = "_convertMeshes.py"


def isInRange(fname, f0, ff):
    fnum = int(fname.strip(".stl").split('_')[1])
    return fnum >= f0 and fnum <= ff
    
def filterSTLFiles(stlFiles, f0, ff):
    simplenames = [ f.split('/')[-1:][0] for f in stlFiles ]

    if f0 == -1:
        return simplenames

    return [f for f in simplenames if isInRange(f, f0, ff)]

def readSTLFiles(dirSTL):
    return glob.glob(dirSTL + "/*.stl")

def generateConversionScript(stlDir, objDir, fileList):
    f = open(SCRIPT_FILENAME, 'w')
    f.write("import bpy\n\n")

    f.write("inPath = '" + stlDir + "' \n")
    f.write("outPath = '" + objDir + "' \n")
    f.write("fileList = ")
    f.write(str(fileList))
        

    f.write(
"""
for mesh in fileList:
    name = mesh.split('.')[0]
    outName = name + ".obj"

    # Deletes all Objects in the start up scene
    # (If you have say the default cube upon open, etc.)                                                                                                                                                 
    bpy.ops.object.select_all(action="SELECT")
    bpy.ops.object.delete()

    # Imports the Mesh to Convert                                                                                                                                                                        
    bpy.ops.import_mesh.stl(filepath = inPath + mesh)
    bpy.ops.export_scene.obj(filepath = outPath + '/' + outName)
""")



    f.close()

def main(argv = None):
    if argv is None:
        argv = sys.argv

    if len(argv) < 3:
        print "Syntax:"
        print "convertMeshes stlDir objDir <startFrame> <endFrame>"
        return 1

    dirSTL = argv[1]
    dirOBJ = argv[2]
    
    startFrame = -1
    endFrame = -1

    if len(argv) == 5:
        startFrame = int(argv[3])
        endFrame = int(argv[4])

    if not os.path.isdir(dirSTL):
        print dirSTL + " is not a valid input path. Exiting."
        return 2

    if not os.path.isdir(dirOBJ):
        print dirOBJ + " does not exist. Creating now."
        os.mkdir(dirOBJ)

    stlFiles = readSTLFiles(dirSTL)
    stlFiles = filterSTLFiles(stlFiles, startFrame, endFrame)

    generateConversionScript(dirSTL, dirOBJ, stlFiles)
    return 0


if __name__ == "__main__":
    sys.exit(main())
