//
//  main.cpp
//  watersound
//
//  Created by Phaedon Sinis on 12/16/13.
//  Copyright (c) 2013 Phaedon Sinis. All rights reserved.
//

#include <iostream>

#include <numeric/FastMultibubble.h>
#include <sound/SoundFrequency.h>
#include <sound/SoundFileManager.h>
#include <geometry/TriangleMesh.h>
#include <sound/SoundTrack.h>
#include <physics/Fluid.h>
#include <physics/PhysicalConstants.h>

using namespace PhysicalConstants;


#include <vector>

static const size_t ZPADLEN = 6;

static const std::string baseDir = "/Users/phaedon/github/aletler/meshes/geomsim2/";

// TODO: replace with utility functions that look in the directory and figure out how many of each
// there are...
static const size_t numBubbles = 500;
static const size_t numFrames = 1000;
static const int frameRate = 200;
//static const size_t numFrames = 40;


int main(int argc, const char * argv[])
{
  Fluid fluid;
  
  SoundTrack st;
  SoundFileManager sfm("/Users/phaedon/fabbubbles3.aiff");
  
  
  // Step forward in time
  for (size_t i = 0; i < numFrames; i++) {
    
    std::cout << "Processing frame " << i << "..." <<  std::endl;
    
    std::vector<TriangleMesh> bubbleMeshes;
    bubbleMeshes.resize(numBubbles);
    TriangleMesh airMesh;
    TriangleMesh solidMesh;
    
    double timeStamp = double(i) / double(frameRate);
    std::string zeroFrameNum = ZeroPadNumber(i, ZPADLEN);
    
    airMesh.read(baseDir + "air_" + zeroFrameNum + ".obj", MFF_OBJ);
    solidMesh.read(baseDir + "solid_" + zeroFrameNum + ".obj", MFF_OBJ);
    
    fluid.setFluidDomain(&airMesh, &solidMesh);

    
    for (size_t b = 0; b < numBubbles; b++) {
      std::string zeroBubNum = ZeroPadNumber(b, ZPADLEN);
      bubbleMeshes[b].clearAll();
      std::string bubbleFilename = baseDir + "bubble_" + zeroBubNum + "_" + zeroFrameNum + ".obj";
      
      bool meshLoaded = bubbleMeshes[b].read(bubbleFilename, MFF_OBJ);
      
      if (meshLoaded) {
        bubbleMeshes[b].flipNormals();
        fluid.setBubble(&bubbleMeshes[b], b, timeStamp);
      }
      
    }
  }
  
  //fluid.printAllFrequencies();
  fluid.integrateAllBubbleSounds();
  
  
  // Now we can interpolate the frequencies for audio time step
  double totalTime = (numFrames / double(frameRate));
  size_t numAudioFrames = ceil(totalTime * Sound::SAMPLING_RATE);
  for (size_t t = 0; t < numAudioFrames; t ++) {

    double totalSample = 0;
    
    for (size_t b = 0; b < numBubbles; b++) {
      totalSample += fluid.getSample(b, t);
    }
    st.addSample(totalSample, 0);
    st.addSample(totalSample, 1);
  }
  
  st.normalize();
  
  sfm.open(WriteOnly);
  sfm.writeAudio(st);
  sfm.close();
  
  
  return 0;
}

