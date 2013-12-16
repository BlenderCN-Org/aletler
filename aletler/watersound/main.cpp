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

#include <vector>


int main(int argc, const char * argv[])
{
  // one object per bubble
  std::vector<SoundFrequency> soundFreqs;
  SoundTrack st;
  SoundFileManager sfm("/Users/phaedon/fabbubbles.aiff");
  int frameRate = 96;
  size_t audioSR = 44100;
  
  // Assuming the fluid has already been simulated, load all meshes for each frame
  // in succession
  size_t nAnimationFrames = 400;

  
  // Step forward in time
  for (size_t i = 0; i < nAnimationFrames; i++) {
    
    std::vector<TriangleMesh> bubbleMeshes;
    TriangleMesh airMesh;
    TriangleMesh solidMesh;
    
    loadMeshes(i); // or something like that
    size_t nBubbles = 10;
    
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
  
  
  return 0;
}

