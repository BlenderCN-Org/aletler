import glob
#import xml.etree.cElementTree as ET
#if this will be used a bunch: consider: http://effbot.org/zone/element.htm
air = {}
solid = {}
bubble = {}
lastScene = 0
'''
Edit these variables below
'''
#where objs are located
#filePath = '/Users/arthur/Desktop/mitTest/geometrySim/'
filePath = '/Users/phaedon/github/aletler/meshes/geomsim2/'
#Where we write the xmls
#outDir = '/Users/arthur/Desktop/mitTest/xmls/'
outDir = '/Users/phaedon/github/aletler/meshes/xmls/'
#This takes us from xmls to geoSim folder

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

for msh in glob.glob(filePath+'*.obj'):
     m = msh[len(filePath):-4].split('_')
     #print m[0]
     #assumes only one air and solid per xml file
     #but multiple bubbles
     if (m[0] == 'air'):
       checkIfLast(m[1])
       air[strToInt(m[1])] = msh
     elif (m[0] == 'solid'):
       checkIfLast(m[1])
       solid[strToInt(m[1])] = msh
     elif (m[0] == 'bubble'):
       checkIfLast(m[2])
       tmp = bubble.get(strToInt(m[2]),[])
       tmp.append(msh)
       bubble[strToInt(m[2])] = tmp
     else:
       pass

for x in range(0,lastScene+1):
  name = 'out'+str(x).zfill(6)+'.xml'
  data = beginScene()
  if (air.has_key(x)):
    data += addShape(air[x],1.333)
  if (solid.has_key(x)):
    data += addShape(solid[x],1.5)
  if (bubble.has_key(x)):
    for b in bubble[x]:
      data += addShape(b,1.333)
  data += endScene()
  f = open(outDir+name,'w')
  f.write(data)
  f.close()

