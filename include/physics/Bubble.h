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
  
  
  void timestep(double dt) {
    _vel += dt * _accel;
    _bubble->translate(dt * _vel);
  }
  
  void setFrequency(double timeStamp, double capacitance) {
    _soundfreq.addFrequency(timeStamp, frequency_omega(capacitance), FREQ_OMEGA);
  }
  
  double frequency_omega(double capacitance) const;
  void setBubbleMesh(TriangleMesh *b);
  void integrateVibrationODE();
  double getSample(size_t sampleIndex);
  
  
  SoundFrequency &soundFrequency() {return _soundfreq;}
 
  
  TriangleMesh *getBubbleMesh() {
    return _bubble;
  }
  
  const SoundFrequency &getSoundFrequency() const {
    return _soundfreq;
  }
  
private:
  size_t _bubble_index;
  
  size_t sample0;
  size_t samplef;
  
  // the initial "radius" of the bubble. Used for computing damping,
  // and set the first time that a bubble mesh is provided.
  double _r0;
  
  std::vector<double> _samples;
  
  SoundFrequency _soundfreq;
  TriangleMesh *_bubble;
  
  Vector3d _vel;
  Vector3d _accel;
  
};


#endif
