//
//  Bubble.h
//  soundmath
//
//  Created by Phaedon Sinis on 11/30/12.
//  Copyright (c) 2012 Phaedon Sinis. All rights reserved.
//

#ifndef soundmath_Bubble_h
#define soundmath_Bubble_h


#include "Monopole.h"
#include <physics/PhysicalConstants.h>


class Bubble {
  
public:
  Bubble(double mmradius, double btime, Vector3d loc) {
    
    m_freq = minnaert(mmradius);
    
    m_mpole.frequency(m_freq);
    m_mpole.position(loc);
    
    m_birthtime = btime;
    m_time = 0;
    m_isFinished = false;
    
    // Initialize (vertical) acceleration & velocity
    // (for now, using simple buoyancy of spherical bubble
    //  to compute acceleration)
    double volume = volume_sphere();
    double mass_air = volume * Fluids::RHO_AIR;
    double mass_water = volume * Fluids::RHO_WATER;
    _a = Dynamics::G * (mass_water - mass_air) / (mass_air + mass_water);
    _v = 0;
    
    
    
  }
  
  double minnaert(double mmradius) {
    
    m_radius = mmradius / 1000.0;
    
    double p0 = Fluids::P_ATM;
    return (0.5 / M_PI) * (1.0 / m_radius) * sqrt(3 * Fluids::GAMMA_AIR * p0 / Fluids::RHO_WATER);
    
  }
  
  void timestep(double dt) {
    
    if (m_isFinished)
      return;
    
    // for now, fwd Euler:
    m_time += dt;
    _v += _a * dt;
    m_depth -= _v * dt;
    
    if (m_depth < m_radius) {
      m_isFinished = true;
    }
    
  }
  
  double pressure(double t, Vector3d ear) {
    if (t < m_birthtime || t > m_birthtime + S_BUBBLELIFE) return 0;
    
    double t_age = t - m_birthtime;
    
    m_freq = frequency();
    m_mpole.frequency(m_freq);
    
    //        double attack = 2 * atan(t_age * 0.5) * M_1_PI;
    double attack = 1;
    double attenuation = 1;
    //        double attenuation = exp(-50 * t_age);
    
    return attack * attenuation * m_mpole.pressure(t_age, ear);
  }
  
  
  double volume_sphere() {
    return 4.0 * M_PI * pow(m_radius, 3) / 3.0;
  }
  
  double pressure_equilibrium(double depth) {
    return pressure_hydrostatic(depth) + pressure_surfacetension();
  }
  
  double pressure_hydrostatic(double depth) {
    return Fluids::P_ATM + Fluids::RHO_WATER * Dynamics::G * depth;
  }
  
  double pressure_surfacetension() {
    return 2 * Fluids::SIGMA_AIR_WATER / m_radius;
  }
  
  
  double capacitance() {
    
    double r2d = m_radius / (2.0 * m_depth);
    
    return m_radius / (1 - r2d - pow(r2d, 4));
    
  }
  
  
  double frequency() {
    
    if (m_isFinished) return 0;
    
    // P0 is bubble pressure at equilibrium:
    double P0 = pressure_equilibrium(m_depth);
    
    // C is capacitance
    //double C = m_radius; // ONLY for a Minnaert bubble!
    double C = capacitance();
    
    // V0 is the "average volume"
    double V0 = volume_sphere();
    
    double numer = 4 * M_PI * Fluids::GAMMA_AIR * P0 * C;
    double denom = Fluids::RHO_WATER * V0;
    
    return (0.5 / M_PI) * sqrt(numer/denom);
    
  }
  
  
  
  
  // accessors
  double get_birthtime() const {
    return m_birthtime;
  }
  
  void set_depth(double depth) {
    m_depth = depth;
  }
  
  double depth() {
    return m_depth;
  }
  
  // predicate function for sorting bubbles
  static
  bool sort_by_birth(const Bubble* a, const Bubble* b) {
    return a->get_birthtime() < b->get_birthtime();
  }
  
  // TODO: generalize. Currently assumes 100ms for bubble duration
  static const double S_BUBBLELIFE;
  
private:
  
  // velocity, acceleration
  double _v, _a;
  
  double m_radius;
  double m_freq;
  
  double m_depth;
  
  Monopole m_mpole;
  
  double m_birthtime;
  
  double m_time;
  
  bool m_isFinished;
};



#endif
