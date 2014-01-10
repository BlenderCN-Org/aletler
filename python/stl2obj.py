'''
A conversion between STL <-> OBJ meshes
Using Blender BPY module 

To run:
  1) supply filePath of folder with Meshes
  2) choose extF (file extension From) and extT (file extension To)
     as .obj or .stl
  3) Navigate to Blender on computer:
      for me its /Applications/blender.app/Contents/MacOS
  4) type 
  ./blender -b --python ~/..PathTo../convert.py 
'''

import glob
import bpy 

#USER CHANGES THESE 3 VARIABLES ONLY
filePath = '/Users/phaedon/fallingJet/rough/'
extF = '.stl'
extT ='.obj'

#If you want to go the other way.
#extF = '.obj'
#extT ='.stl'



for mesh in glob.glob(filePath+'*'+extF):
  outName = mesh[:-len(extF)]+extT
  #Deletes all Objects in the start up scene
  #(If you have say the default cube upon open, etc.)
  bpy.ops.object.select_all(action="SELECT")
  bpy.ops.object.delete()
  #Imports the Mesh to Convert
  if extF == '.stl':
    bpy.ops.import_mesh.stl(filepath=mesh)
    #Sets the name
    bpy.ops.export_scene.obj(filepath=outName)
  elif extF == '.obj':
    bpy.ops.import_scene.obj(filepath=mesh)
    bpy.ops.export_mesh.stl(filepath=outName)
  else:
    raise Exception ('Did not specify obj or stl')
