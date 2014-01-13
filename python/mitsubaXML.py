#!/usr/bin/python

import glob
import os, sys

#import xml.etree.cElementTree as ET
#if this will be used a bunch: consider: http://effbot.org/zone/element.htm
air = {}
solid = {}
bubble = {}
lastScene = 0


def beginScene ():
  return   '<?xml version="1.0" encoding="utf-8"?>\n' \
           '<scene version="0.4.4"> \n\n' \
           '<integrator type="photonmapper">\n' \
           '<integer name="causticPhotons" value="400000"/>\n' \
           '</integrator>\n\n'
def addShape (name,value):
  return  '<shape type="obj"> \n' \
          '<string name="filename" value="' + name +'"/>\n' \
          '<bsdf type="dielectric">\n' \
          '<float name="intIOR" value="'+ str(value) +'"/> </bsdf>\n' \
          '</shape>\n\n' 
  return 

def endScene():
  return '</scene>'

def checkIfLast(num):
  global lastScene
  #take off leading 0's
  n = strToInt(num)
  if (n > lastScene):
    lastScene = n
  return None

def strToInt(num):
  num = num.lstrip('0')
  if (num == ''):
    num = 0
  return int(num)


def main(argv = None):
  if argv is None:
    argv = sys.argv
    
  dirOBJ = argv[1]
  dirXML = argv[2]

  if not os.path.isdir(dirOBJ):
    print dirOBJ + " is not a valid input path. Exiting."
    return 2

  if not os.path.isdir(dirXML):
    print dirXML + " does not exist. Creating now."
    os.mkdir(dirXML)
  

  objFileList = glob.glob(dirOBJ + '*.obj')

  sceneNumbers = []
  
  for msh in objFileList:
    objFile = msh.split('/')[-1:][0]
    m = objFile.strip(".obj").split('_')

    scene = int(m[1])
    if scene not in sceneNumbers:
      sceneNumbers.append(scene)
    
     #assumes only one air and solid per xml file
     #but multiple bubbles
    if (m[0] == 'smoothedInterface'):
      checkIfLast(m[1])
      air[strToInt(m[1])] = msh
    elif (m[0] == 'bubbles'):
      checkIfLast(m[1])
      solid[strToInt(m[1])] = msh
    elif (m[0] == 'garbage'):
      checkIfLast(m[2])
      tmp = bubble.get(strToInt(m[2]),[])
      tmp.append(msh)
      bubble[strToInt(m[2])] = tmp
    else:
      pass

  for x in sceneNumbers:
    
    name = 'out'+str(x).zfill(6)+'.xml'
    data = beginScene()
    if (air.has_key(x)):
      data += addShape(air[x],1.333)
    if (solid.has_key(x)):
      #data += addShape(solid[x],1.5)
      data += addShape(solid[x],1.333) # since I changed "solid" to "bubbles"
    if (bubble.has_key(x)):
      for b in bubble[x]:
        data += addShape(b,1.333)
    data += endScene()

    f = open(dirXML + name,'w')
    f.write(data)
    f.close()
  

  return 0

if __name__ == "__main__":
  sys.exit(main())
