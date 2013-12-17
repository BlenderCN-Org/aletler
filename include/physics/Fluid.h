//
//  Fluid.h
//  aletler
//
//  This class handles the entire domain, bubbles included, during a single timestep.
//  It communicates with the Electrostatics / FastMultibubble interfaces to ensure that
//  computations are not redundant
//

#ifndef __aletler__Fluid__
#define __aletler__Fluid__


#include "Electrostatics.h"

#include <iostream>
#include <geometry/TriangleMesh.h>
#include <numeric/FastMultibubble.h>


class Fluid {
  
public:
  Fluid() {}
  
  void setFluidDomain(TriangleMesh *air, TriangleMesh *solid) {
    _air = air;
    _solid = solid;
    
    e.setDomain(_air, _solid);
  }
  
  void setBubble(TriangleMesh *bub) {
    _bubble = bub;
    e.setBubble(_bubble);
  }
  
  double singleBubbleFrequency() {
    return e.bubbleCapacitance();
  }
  
private:
  Electrostatics e;
  
  TriangleMesh *_air;
  TriangleMesh *_solid;
  TriangleMesh *_bubble;
  
  
};


#endif /* defined(__aletler__Fluid__) */
