//
//  Fluid.cpp


#include "Fluid.h"
#include <sstream>
#include <fstream>
#include <iomanip> // set precision on output


//velocityFilename

void Fluid::saveAirVelocityFile(const std::string &fullFilename, const VectorXd &velAir) {
  
  std::ofstream ofile;
  ofile.open(fullFilename.c_str());
  
  ofile
  << std::setprecision(10)
  << std::fixed;
  
  for (size_t i = 0; i < velAir.size(); i++) {
    ofile << velAir(i) << std::endl;
  }
  
  ofile.close();
}
