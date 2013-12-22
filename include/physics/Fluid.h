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
#include <geometry/TriangleMesh.h>
#include "Bubble.h"

#include <boost/math/special_functions/fpclassify.hpp>

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
    
    double bubCap = e.bubbleCapacitance();
    if (isnan(bubCap)) {
      std::cout << "capacitance is NaN" << std::endl;
    } else {
      _bubbles[bubbleIndex].setFrequency(timeStamp, bubCap);
    }
  }
  
  void printAllFrequencies() const {
    std::cout << "Printing frequencies for " << _bubbles.size() << " bubbles." << std::endl;
    for (size_t b = 0; b < _bubbles.size(); b++) {
      std::cout << "BUBBLE # " << b << ":" << std::endl;
      _bubbles[b].getSoundFrequency().printFrequencyVector();
    }
  }
  
  void integrateAllBubbleSounds() {
    for (size_t b = 0; b < _bubbles.size(); b++) {
      std::cout << "Integrating ODE for bubble # " << b << std::endl;
      _bubbles[b].integrateVibrationODE();
    }
  }
  
  double getSample(size_t bubbleNum, size_t audioFrame) {
    return _bubbles[bubbleNum].getSample(audioFrame);
  }
  
  
private:
  Electrostatics e;
  
  std::vector<Bubble> _bubbles;
  
  TriangleMesh *_air;
  TriangleMesh *_solid;
  TriangleMesh *_bubble;
  
  
};


#endif /* defined(__aletler__Fluid__) */
