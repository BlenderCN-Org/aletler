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
    
    _combined.clearAll();
    _combined.boundaryType = FBT_AIR;
    _combined.append(_air);
    _combined.boundaryType = FBT_SOLID;
    _combined.append(_solid);
  }

  void setBubble(TriangleMesh *bub, size_t bubbleIndex, double timeStamp,
                 std::string &fastBEMfilename,
                 std::string &velocityFilename) {
    
    if (bubbleIndex >= _bubbles.size()) {
      _bubbles.resize(bubbleIndex + 1);
    }

    _bubbles[bubbleIndex].setBubbleMesh(bub);
    
    _bubble = bub;
    e.setBubble(_bubble);
    
    VectorXd velAir;
    double bubCap = e.bubbleCapacitance(velAir);
    if (isnan(bubCap)) {
      std::cout << "capacitance is NaN" << std::endl;
    } else {

      float freq_hz = _bubbles[bubbleIndex].setFrequency(timeStamp, bubCap);
      _combined.writeFastBEM(fastBEMfilename, velAir, freq_hz);
      
      saveAirVelocityFile(velocityFilename, velAir);
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
  
  void saveBubbleFrequencyFile(size_t bubbleIndex) const {
    _bubbles[bubbleIndex].saveBubbleFrequencyFile();
  }
  
  void saveAirVelocityFile(const std::string &fullFilename, const VectorXd &velAir);
  
private:
  Electrostatics e;
  
  std::vector<Bubble> _bubbles;
  
  TriangleMesh *_air;
  TriangleMesh *_solid;
  TriangleMesh *_bubble;
  
  TriangleMesh _combined;
  
};


#endif /* defined(__aletler__Fluid__) */
