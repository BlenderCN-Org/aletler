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


#include <vector>

static const size_t ZPADLEN = 6;

static const std::string baseDir = "/Users/phaedon/github/aletler/meshes/geometrySim/";

// TODO: replace with utility functions that look in the directory and figure out how many of each
// there are...
static const size_t numBubbles = 20;
static const size_t numFrames = 480;

// one object per bubble
static std::vector<SoundFrequency> soundFreqs;



int main(int argc, const char * argv[])
{
  Fluid fluid;
  
  soundFreqs.resize(numBubbles);
  
  SoundTrack st;
  SoundFileManager sfm("/Users/phaedon/fabbubbles.aiff");
  int frameRate = 96;
  size_t audioSR = 44100;
  
  
  // Step forward in time
  for (size_t i = 0; i < numFrames; i++) {
    
    std::vector<TriangleMesh> bubbleMeshes;
    bubbleMeshes.resize(numBubbles);
    TriangleMesh airMesh;
    TriangleMesh solidMesh;
    
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
        fluid.setBubble(&bubbleMeshes[b]);
        double freq = fluid.singleBubbleFrequency();
        std::cout << "BUBBLE " << b << ": " << freq << std::endl;
      }
      
    }
  }
  
    /*
    // Loop over all bubbles in current frame
    for (size_t b = 0; b < nBubbles; b++) {
      double freq = computeBubbleFrequency(bubbleMeshes[b]);
      soundFreqs[b].addFrequency(double(i)/double(frameRate), freq);
    }
  }
  
  // Now we can interpolate the frequencies for audio time step
  for (double t = 0; t < (nAnimationFrames / double(frameRate)) * audioSR; t += 1 / audioSR) {

    double totalSample = 0;
    for (size_t i = 0; i < soundFreqs.size(); i++) {
      totalSample += soundFreqs[i].frequencyAt(t);
    }
    st.addSample(totalSample, 0);
  }
  
  st.normalize();
  
  sfm.open(SFM_WRITE);
  sfm.writeAudio(st);
  sfm.close();
  */
  
  return 0;
}

