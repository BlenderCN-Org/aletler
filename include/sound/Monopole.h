//
//  Monopole.h
//  soundmath
//
//  Created by Phaedon Sinis on 11/30/12.
//  Copyright (c) 2012 Phaedon Sinis. All rights reserved.
//

#ifndef soundmath_Monopole_h
#define soundmath_Monopole_h


#include "SoundParameters.h"
#include "SphericalTools.h"
#include <Eigen/Dense>
#include <complex>


using namespace Eigen;
using namespace std;

class Monopole {
public:
  Monopole() {}
  
  Monopole(double freq) {
    m_freq = freq;
    m_omega = freq * 2 * M_PI;
    m_k = m_omega / C_AIR;
    
    m_source_pos = Vector3d(0, 0, 0);
    //m_source_vel = Vector3d(0, 14, 0);
    m_source_vel = Vector3d(0, 0, 0);
  }
  
  double pressure(double t, Vector3d ear) const {
    
    if (m_freq == 0.0) return 0;
    
    complex<double> g = greens(ear);
    
    complex<double> complex_press = -ii * m_k * C_AIR * RHO_AIR * g * exp(-ii * m_omega * t);
    
    return complex_press.real();
  }
  
  
  // Update position
  void position(Vector3d pos) { m_source_pos = pos; }
  
  void step(double dt) {
    m_source_pos += dt * m_source_vel;
  }
  
  // Update freq
  void frequency(double freq) {
    m_freq = freq;
    m_omega = freq * 2 * M_PI;
    m_k = m_omega / C_AIR;
  }
  
private:
  Vector3d m_source_pos;
  Vector3d m_source_vel;
  
 // double t;
  
  // Frequency
  double m_freq;
  
  // Ang freq
  double m_omega;
  
  // Wavenumber
  double m_k;
  
  complex<double> greens_expansion(Vector3d ear) const {
    
    complex<double> sphharmsum = 0.0;
    
    // TODO: magic const
    for (int n = 0; n < 4; n++) {
      for (int m = -n; m <= n; m++) {
        sphharmsum += SphericalTools::S(-m, n, m_k, m_source_pos)
        * SphericalTools::R(m, n, m_k, ear);
      }
    }
    
    return ii * m_k * sphharmsum;
  }
  
  complex<double> greens(Vector3d ear) const {
    Vector3d r = (ear - m_source_pos);
    return SphericalTools::Greens(m_k, r);
  }
};

#endif
