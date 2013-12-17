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
#include "Bubble.h"

class Fluid {
  
public:
  Fluid() {}
  
  void setFluidDomain(TriangleMesh *air, TriangleMesh *solid) {
    _air = air;
    _solid = solid;
    
    e.setDomain(_air, _solid);
  }

  void setBubble(TriangleMesh *bub, size_t bubbleIndex, double timeStamp) {
    
    if (bubbleIndex >= _bubbles.size()) {
      _bubbles.resize(bubbleIndex + 1);
    }

    _bubbles[bubbleIndex].setBubbleMesh(bub);
    
    _bubble = bub;
    e.setBubble(_bubble);
    
    _bubbles[bubbleIndex].setFrequency(timeStamp, e.bubbleCapacitance());
  }
  
  const void printAllFrequencies() const {
    std::cout << "Printing frequencies for " << _bubbles.size() << " bubbles." << std::endl;
    for (size_t b = 0; b < _bubbles.size(); b++) {
      std::cout << "BUBBLE # " << b << ":" << std::endl;
      _bubbles[b].getSoundFrequency().printFrequencyVector();
    }
  }
  
  
private:
  Electrostatics e;
  
  std::vector<Bubble> _bubbles;
  
  TriangleMesh *_air;
  TriangleMesh *_solid;
  TriangleMesh *_bubble;
  
  
};


#endif /* defined(__aletler__Fluid__) */
