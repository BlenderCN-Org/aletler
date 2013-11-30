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

class Bubble {

public:
  Bubble() {
    _surface = new TriangleMesh;
    _bubble = new TriangleMesh;
  }
  
  ~Bubble() {
    delete _surface;
    delete _bubble;
  }
  
 

  void set_directory(std::string _dir) {
    _directory = _dir;
  }
  
  
  double capacitance() {
    
    Electrostatics e;
    e.setSurface(_surface);
    e.addBubble(_bubble);
    
    std::vector<double> c;
    e.capacitance(c);
    
    return c[0];
  }
  
  
  void io_loadMeshes(const std::string &surface_name,
                     const std::string &bubble_name,
                     size_t bubble_index,
                     size_t frame_index,
                     int num_digits_bubble,
                     int num_digits_frame);
private:

  std::string _directory;
  size_t _bubble_index;
  
  TriangleMesh *_surface;
  TriangleMesh *_bubble;
  
};


#endif
