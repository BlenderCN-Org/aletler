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
#include <geometry/TriangleMesh.h>
#include "Electrostatics.h"
#include "PhysicalConstants.h"
#include <sound/SoundFrequency.h>
#include <vector>


#include <iomanip>
#include <sstream>

using namespace PhysicalConstants;


class Bubble {
  
public:
  
  Bubble() :
  _bubble_index(0),
  _vel(0,0,0),
  _accel(0,0,0)
  
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
  
  double frequency_omega(double capacitance) const {

    if (capacitance == 0) return 0;

    double four_pi_gamma = 4 * M_PI * PhysicalConstants::Fluids::GAMMA_AIR;
    
    double v = _bubble->volume();
    
    // TODO: doens't take hydrostatic pressure into account
    double p0 = PhysicalConstants::Fluids::P_ATM;
    
    return sqrt( four_pi_gamma * p0 * capacitance / (PhysicalConstants::Fluids::RHO_WATER * v) );
  }
  
  
  
  SoundFrequency &soundFrequency() {return _soundfreq;}
  
  /*
  void load(size_t frame_index) {
    
    _surface->clearAll();
    _bubble->clearAll();
    _solid->clearAll();
    io_loadMeshes("free_surface_glass", "bubble_lr_",
                  _bubble_index, frame_index,
                  6, 6);
    
    double bubbleVolume = _bubble->volume();
    double mass_air = bubbleVolume * Fluids::RHO_AIR;
    double mass_water = bubbleVolume * Fluids::RHO_WATER;
    _accel = Vector3d(0, Dynamics::G * (mass_water - mass_air) / (mass_air + mass_water), 0);
  }
   */
  
  void setBubbleMesh(TriangleMesh *b) {
    _bubble = b;
    double bubbleVolume = _bubble->volume();
    double mass_air = bubbleVolume * Fluids::RHO_AIR;
    double mass_water = bubbleVolume * Fluids::RHO_WATER;
    _accel = Vector3d(0, Dynamics::G * (mass_water - mass_air) / (mass_air + mass_water), 0);

  }
  
  TriangleMesh *getBubbleMesh() {
    return _bubble;
  }
  
  const SoundFrequency &getSoundFrequency() const {
    return _soundfreq;
  }
  
private:
  size_t _bubble_index;
  
  SoundFrequency _soundfreq;
  TriangleMesh *_bubble;
  
  Vector3d _vel;
  Vector3d _accel;
  
};


#endif
