//
//  BoundaryPressure.cpp
//  aletler
//
//  Created by Phaedon Sinis on 1/2/14.
//  Copyright (c) 2014 Phaedon Sinis. All rights reserved.
//

#include "BoundaryPressure.h"
#include <vdb.h>
#include <fstream>


template <typename T>
T StringToNumber (const std::string &Text ) {
  //character array as argument
  std::stringstream ss(Text);
  T result;
  return ss >> result ? result : 0;
}

void BoundaryPressure::read(const std::string &filename) {
  std::ifstream ifile(filename.c_str());
  std::string line;
  
  const std::string& whitespace = " \t";
  if (ifile.is_open()) {
    while (std::getline (ifile,line)) {
  
      size_t strBegin = line.find_first_not_of(whitespace);
      if (strBegin == std::string::npos) // empty line
        continue;
      
      if (line[strBegin] == '~' || line[strBegin] == 'F' || line[strBegin] == 'E')
        continue;
      
      std::stringstream stream(line);
      std::string realStr, imStr;
      std::string junk;
      stream >> junk; // index
      stream >> junk; // open paren
      stream >> realStr;
      stream >> imStr;

      double realD = StringToNumber<double>(realStr);
      double imD = StringToNumber<double>(imStr);
      
      std::cout << realD << "   " << imD << std::endl;
      
      _cpressures.push_back(std::complex<double>(realD, imD));
      
    }
    ifile.close();
  }
  
  computeModuli();
}



void BoundaryPressure::visualize(const TriangleMesh &combined) const {
  
  vdb_frame();
  
  float span = _moduli.maxCoeff() - _moduli.minCoeff();
  
  for (size_t i = 0; i < combined.size(); i++) {
    Triangle t = combined.triangle(i);
    Vector3d c = t.centroid();
    
    float r = (_moduli(i) - _moduli.minCoeff()) / span;
    float g = 0.0;
    float b = 1.0 - r;
    
    vdb_color(r,g,b);
    vdb_point(c.x(), c.y(), c.z());
  }
  
  
}