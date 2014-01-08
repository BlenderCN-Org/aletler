//
//  BoundaryPressure.h
//  aletler
//
//  Created by Phaedon Sinis on 1/2/14.
//  Copyright (c) 2014 Phaedon Sinis. All rights reserved.
//

#ifndef __aletler__BoundaryPressure__
#define __aletler__BoundaryPressure__

#include <iostream>
#include <complex>
#include <vector>

#include <geometry/TriangleMesh.h>

class BoundaryPressure {
public:

  void read(const std::string &filename);
  
  void visualize(const TriangleMesh &combined) const;
  
private:
  
  void computeModuli() {
    _moduli.resize(_cpressures.size(), 1);
    
    for (size_t i = 0; i < _cpressures.size(); i++) {
      _moduli(i) = sqrt( _cpressures[i].real() * _cpressures[i].real()
                       + _cpressures[i].imag() * _cpressures[i].imag() );
    }
  }
  
  std::vector<std::complex<double> > _cpressures;
  VectorXd _moduli;
};

#endif /* defined(__aletler__BoundaryPressure__) */
