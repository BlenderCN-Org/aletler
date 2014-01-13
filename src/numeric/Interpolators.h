//
//  Interpolators.h
//  aletler
//
//  Created by Phaedon Sinis on 1/13/14.
//  Copyright (c) 2014 Phaedon Sinis. All rights reserved.
//

#ifndef aletler_Interpolators_h
#define aletler_Interpolators_h


// Interp y corresponding to x, depending on distance between x0 and x1
static double interpLinearDouble(double x,
                                 double x0, double x1,
                                 double y0, double y1) {
  
  double a = (x - x0) / (x1 - x0);
  
  return y0 * (1 - a) + y1 * (a);
}



// Interp y corresponding to x, depending on distance between x0 and x1
static std::complex<double> interpLinearComplex(double x,
                                                double x0, double x1,
                                                std::complex<double> y0, std::complex<double> y1) {
  

  return std::complex<double>(interpLinearDouble(x, x0, x1, y0.real(), y1.real()),
                              interpLinearDouble(x, x0, x1, y0.imag(), y1.imag()));
}






// Assumes xvec is sorted
// Interp y corresponding to x, based on sequence of values in xvec & yvec
// If outside of range, returns whatever value user specifies as "out_of_range"
// (optional parameter)
static double interpLinearVectors(double x,
                                  const std::vector<double> &xvec,
                                  const std::vector<double> &yvec,
                                  double out_of_range = 0) {
  
  if (xvec.empty()
      || x < xvec[0] || x > xvec.back()) {
    return out_of_range;
  }
  
  if (xvec.size() != yvec.size()) {
    std::cout << "Attempting to interp two vectors of different sizes. Failing." << std::endl;
    return out_of_range;
  }
  
  
  for (size_t i = 1; i < xvec.size(); i++) {
    double xcurr = xvec[i - 1];
    double xnext = xvec[i];
    
    double ycurr = yvec[i - 1];
    double ynext = yvec[i];
    
    
    if (x == xcurr)
      return ycurr;
    if (x == xnext)
      return ynext;
    
    if (x > xcurr && x < xnext) {
      return interpLinearDouble(x,
                                xcurr, xnext,
                                ycurr, ynext);
    }
  }
  
  // Never reached because of program logic. This is here to avoid
  // compiler errors:
  return yvec.back();
}


// Assumes xvec is sorted
// Interp y corresponding to x, based on sequence of values in xvec & yvec
// If outside of range, returns whatever value user specifies as "out_of_range"
// (optional parameter)
static std::complex<double> interpLinearComplexVectors(double x,
                                                const std::vector<double> &xvec,
                                                const std::vector<std::complex<double> > &yvec,
                                                std::complex<double> out_of_range = std::complex<double>(0,0)) {
  
  if (xvec.empty()
      || x < xvec[0] || x > xvec.back()) {
    return out_of_range;
  }
  
  if (xvec.size() != yvec.size()) {
    std::cout << "Attempting to interp two vectors of different sizes. Failing." << std::endl;
    return out_of_range;
  }
  
  
  for (size_t i = 1; i < xvec.size(); i++) {
    double xcurr = xvec[i - 1];
    double xnext = xvec[i];
    
    std::complex<double> ycurr = yvec[i - 1];
    std::complex<double> ynext = yvec[i];
    
    
    if (x == xcurr)
      return ycurr;
    if (x == xnext)
      return ynext;
    
    if (x > xcurr && x < xnext) {
      return interpLinearComplex(x,
                                xcurr, xnext,
                                ycurr, ynext);
    }
  }
  
  // Never reached because of program logic. This is here to avoid
  // compiler errors:
  return yvec.back();
}

#endif
