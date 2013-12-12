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
#include <vector>

class Bubble {

public:
  Bubble(size_t idx) : _bubble_index(idx) {
    _surface = new TriangleMesh;
    _bubble = new TriangleMesh;
  }
  
  /*
  ~Bubble() {
    delete _surface;
    delete _bubble;
  }*/
  
  double frequency_omega() const {
    double four_pi_gamma = 4 * M_PI * PhysicalConstants::Fluids::GAMMA_AIR;
    
    double c = capacitance();
    if (c == 0) return 0;
    
    double v = _bubble->volume();
    
    // TODO: doens't take hydrostatic pressure into account
    double p0 = PhysicalConstants::Fluids::P_ATM;
    
    return sqrt( four_pi_gamma * p0 * c / (PhysicalConstants::Fluids::RHO_WATER * v) );
  }
  
  double frequency_hz() const {
    return (0.5 * M_1_PI) * frequency_omega();
  }
  
  void compute_all_frequencies(size_t first_frame, size_t last_frame) {
    _freqs.resize(last_frame, 0);
    for (size_t i = first_frame; i <= last_frame; i++) {
      load(i);
      _freqs[i] = frequency_hz();
    }
  }
  
  double get_frequency(size_t framenum) {
    if (framenum < _freqs.size()) {
      return _freqs[framenum];
    } else {
      return 0.0;
    }
  }
  
  // interpolates for the specific timestamp
  // framerate: (animation) frames per second
  double get_frequency(double timestamp, int framerate) {
    // figure out surrounding frame numbers:
    double frac_fn = timestamp * framerate;
    int prev_fn = floor(frac_fn);
    int next_fn = ceil(frac_fn);
    
    //std::cout << "FRAME NUM: " << prev_fn << " " << frac_fn << "   " << next_fn << std::endl;
    
    double prev_freq = get_frequency(prev_fn);
    double next_freq = get_frequency(next_fn);

    if (prev_fn == next_fn)
      return prev_freq;
    
    // edge cases: don't interpolate with 0
    if (prev_freq == 0.0 || next_freq == 0.0) return 0.0;
    
    // interp ratio
    double a = (frac_fn - prev_fn) / (double)(next_fn - prev_fn);
    
    double log_pf = log(prev_freq);
    double log_nf = log(next_freq);
    
    // interpolate on the log of the (hertz) frequencies
    double interp_log_freq = log_pf * (1-a) + log_nf * a;
    return exp(interp_log_freq);
  }
 

  void set_directory(std::string _dir) {
    _directory = _dir;
  }
  
  void load(size_t frame_index) {
    
    _surface->clearAll();
    _bubble->clearAll();
    
    io_loadMeshes("surface", "b",
                  _bubble_index, frame_index,
                  6, 6);
  }
  
  double capacitance() const {
    
    Electrostatics e;
    e.setSurface(_surface);
    e.setBubble(_bubble);

    return e.bubbleCapacitance();
  }
  
private:

  void io_loadMeshes(const std::string &surface_name,
                     const std::string &bubble_name,
                     size_t bubble_index,
                     size_t frame_index,
                     int num_digits_bubble,
                     int num_digits_frame);

  std::string _directory;
  size_t _bubble_index;
  
  std::vector<double> _freqs;
  
  TriangleMesh *_surface;
  TriangleMesh *_bubble;
  
  
  
};


#endif
