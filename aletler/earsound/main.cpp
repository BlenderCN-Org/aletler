//
//  main.cpp
//  earsound
//
//  Created by Phaedon Sinis on 1/8/14.
//  Copyright (c) 2014 Phaedon Sinis. All rights reserved.
//

#include <iostream>
#include <physics/Bubble.h>
#include <io/FileNameGen.h>
#include <sound/SoundTrack.h>
#include <sound/SoundFileManager.h>
#include <physics/PhysicalConstants.h>
using namespace PhysicalConstants;

static const size_t numFrames = 200;
static const int frameRate = 200;
static const size_t g_numBubbles = 1;
static const std::string baseDir("/Users/phaedon/github/aletler/meshes/geomsim3/");

static std::vector<Bubble> g_bubbles;

int main(int argc, const char * argv[]) {

  // load bubbles
  g_bubbles.resize(g_numBubbles);
  
  for (size_t b = 0; b < g_numBubbles; b++) {
    
    g_bubbles[b].setAnimFrameRate(frameRate);

    std::string filename = bubbleFreqFilename(baseDir, "bubblefreqs", b);
    g_bubbles[b].loadBubbleFrequencyFile(filename);
    g_bubbles[b].integrateVibrationODE();

  }
  
  /*******/
  // Here, we load the results of the external BEM solver (the weights)
  // and also the air-surface velocities
  
  Eigen::VectorXd weights;
  Eigen::VectorXd airVel;
  
  for (size_t f = 0; f < numFrames; f++) {
    
    for (size_t b = 0; b < g_numBubbles; b++) {
      const std::string wtSubDir = "bemout";
      const std::string avSubDir = "velocities";
      
      const std::string velFile = velocityFilename(baseDir, avSubDir, b, f);
      const std::string bemFile = outBEMFilename(baseDir, wtSubDir, b, f);
      
      g_bubbles[b].loadExternalSolverFiles(bemFile, velFile);
    }
  }
  /*******/
  
  
  
  SoundTrack st;
  SoundFileManager sfm("/Users/phaedon/fabbubbles6.aiff");
  
  
  // Now we can interpolate the frequencies for audio time step
  double totalTime = (numFrames / double(frameRate));
  size_t numAudioFrames = ceil(totalTime * Sound::SAMPLING_RATE);
  for (size_t t = 0; t < numAudioFrames; t ++) {
    
    double totalSample = 0;
    
    for (size_t b = 0; b < g_numBubbles; b++) {
      totalSample += g_bubbles[b].getSample(t);
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

