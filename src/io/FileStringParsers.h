//
//  FileStringParsers.h
//  aletler
//
//  Created by Phaedon Sinis on 1/13/14.
//  Copyright (c) 2014 Phaedon Sinis. All rights reserved.
//

#ifndef aletler_FileStringParsers_h
#define aletler_FileStringParsers_h

#include <sstream>



template <typename T>
T StringToNumber (const std::string &Text ) {
  //character array as argument
  std::stringstream ss(Text);
  T result;
  return ss >> result ? result : 0;
}



static void StringToDoublePair(const std::string &line,
                               double &t,
                               double &f ) {
  
  std::stringstream stream(line);
  std::string ts, fs;
  stream >> ts;
  stream >> fs;
  
  t = StringToNumber<double>(ts);
  f = StringToNumber<double>(fs);
}




static std::complex<double> StringToComplex(const std::string &line) {

  std::string realStr, imagStr;
  std::stringstream stream(line);
  
  stream >> realStr;
  stream >> imagStr;
  
  return std::complex<double>(StringToNumber<double>(realStr),
                              StringToNumber<double>(imagStr));
  
  
}




#endif
