//
//  Bubble.h
//  aletler
//
//  Created by Phaedon Sinis on 11/29/13.
//  Copyright (c) 2013 Phaedon Sinis. All rights reserved.
//

#ifndef aletler_Bubble_h
#define aletler_Bubble_h

#include <string>
#include <vector>

#include <geometry/TriangleMesh.h>
#include <sound/SoundFrequency.h>

class Bubble {
  
public:
  
  Bubble() :
  _bubble_index(0),
  _vel(0,0,0),
  _accel(0,0,0),
  _r0(-1) // flag for "not yet set"
  
  {
    _bubble = new TriangleMesh;
  
  }
  
  
  void timestep(double dt);
  
  // returns the frequency, in Hz
  double setFrequency(double timeStamp, double capacitance);
  
  double frequency_omega(double capacitance) const;
  void setBubbleMesh(TriangleMesh *b);
  void integrateVibrationODE();
  double getSample(size_t sampleIndex);
  
  
  TriangleMesh *getBubbleMesh() {
    return _bubble;
  }
  
  const SoundFrequency &getSoundFrequency() const {
    return _soundfreq;
  }
  
  void saveBubbleFrequencyFile() const;
  
  bool loadBubbleFrequencyFile(const std::string &filename);
  
  void loadExternalSolverFiles(const std::string &weightFilename,
                               const std::string &velFilename);
  
  
  void setAnimFrameRate(size_t fr) { _animFrameRate = fr; }
  
private:
  
  double getPressureScale(double time);
  
  size_t _bubble_index;
  
  // These are the first and last SOUND samples (i.e. at 44100 kHz)
  // and can be used to index into the _samples vector
  size_t sample0;
  size_t samplef;
  
  // the initial "radius" of the bubble. Used for computing damping,
  // and set the first time that a bubble mesh is provided.
  double _r0;
  
  std::vector<double> _samples;
  
  SoundFrequency _soundfreq;
  TriangleMesh *_bubble;
  
  // The ANIMATION frame rate at which external pressure scalar samples are generated.
  size_t _animFrameRate;
  
  //std::vector<size_t> _pressureFrames;
  std::vector<double> _pressureScales;
  
  Vector3d _vel;
  Vector3d _accel;
  
};


#endif
