//
//  SoundFrequency.h
//  aletler
//
//  Created by Phaedon Sinis on 12/13/13.
//  Copyright (c) 2013 Phaedon Sinis. All rights reserved.
//

#ifndef aletler_SoundFrequency_h
#define aletler_SoundFrequency_h

#include <vector>
#include <fstream>
#include <iomanip>  // std::setprecision
#include <numeric/Interpolators.h>

enum FrequencyType {
  FREQ_HERTZ,
  FREQ_OMEGA
};

// An interface to a vector of frequencies along with interpolation
class SoundFrequency {

public:
  
  void addFrequency(double t, double freq, FrequencyType ft = FREQ_HERTZ) {
    _times.push_back(t);
    
    if (ft == FREQ_HERTZ) {
      _frequencies.push_back(freq * 2 * M_PI);
    } else {
      _frequencies.push_back(freq);
    }
  }
  
  double frequencyAt(double t, FrequencyType ft = FREQ_HERTZ) {
    
    double freq = interpLinearVectors(t, _times, _frequencies);
    
    if (ft == FREQ_HERTZ)
      return freq * 0.5 * M_1_PI;
    else
      return freq;    
  }
  
  void printFrequencyVector() const {
    for (size_t i = 0; i < _times.size(); i++) {
      std::cout << "t " << _times[i] << "\tf " << _frequencies[i] << std::endl;
    }
  }
  
  double startTime() const { return _times[0]; }
  double stopTime() const { return _times[_times.size() - 1]; }
  
  bool isEmpty() { return _times.size() == 0; }
  
  void saveFrequencyFile(std::ofstream &ofile) const {
    //std::ofstream ofile;
    //ofile.open(filename.c_str());
    
    for (size_t i = 0; i < _frequencies.size(); i++) {
      ofile << std::setprecision(10)
      << std::fixed
      << _times[i] << " "
      << _frequencies[i] << std::endl;
    }
    
   // ofile.close();
  }
  
private:
  std::vector<double> _times;
  
  // stored here: the omegas (natural frequencies), not Hertz:
  std::vector<double> _frequencies;
  
};

#endif
