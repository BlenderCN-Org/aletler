//
//  main.cpp
//  aletler
//
//  Created by Phaedon Sinis on 11/29/13.
//  Copyright (c) 2013 Phaedon Sinis. All rights reserved.
//

#include <physics/Bubble.h>
#include <physics/Electrostatics.h>
#include <iostream>

#include <sound/Monopole.h>
#include <sound/SoundTrack.h>
#include <sound/SoundFileManager.h>

/******** BOOST STUFF **********/
#include <boost/numeric/odeint.hpp>
using namespace boost::numeric::odeint;
void rhs( const double x , double &dxdt , const double t )
{
  dxdt = 3.0/(2.0*t*t) + x/(2.0*t);
}

void write_cout( const double &x , const double t )
{
  cout << t << '\t' << x << endl;
}

// state_type = double
typedef runge_kutta_dopri5< double > stepper_type;

void boostmain()
{
  double x = 0.0;
  integrate_adaptive( make_controlled( 1E-12 , 1E-12 , stepper_type() ) ,
                     rhs , x , 1.0 , 10.0 , 0.1 , write_cout );
}
/******** BOOST STUFF **********/


int main() {
  
  boostmain();
  return 1;
  
  Bubble b0(0);
  Bubble b1(1);
  Bubble b2(2);
  Bubble b3(3);
  
  std::string bubbledir = "/Users/phaedon/github/bem-laplace-simple/meshes/bubbles";
  
  b0.set_directory(bubbledir);
  b1.set_directory(bubbledir);
  b2.set_directory(bubbledir);
  b3.set_directory(bubbledir);

  b0.compute_all_frequencies(0, 24);
 // b1.compute_all_frequencies(0, 48);
  //b2.compute_all_frequencies(0, 48);
  //b3.compute_all_frequencies(0, 48);

  /*
  for (size_t i = 0; i < 48; i++) {
    std::cout << "Freq: "
    << b0.get_frequency(i) << "  "
    << b1.get_frequency(i) << "  "
    << b2.get_frequency(i) << "  "
    << b3.get_frequency(i) << "  "
    << std::endl;
  }
  */
  
  Monopole m0, m1, m2, m3;

  double inv_SR = 1.0 / 44100;
  Vector3d earL (1,1,1);
  Vector3d earR (1,1,1);

  SoundTrack st(44100, 2);
  SoundFileManager sfm("/Users/phaedon/bubbles.aiff");
  
  
  for (double i = 0.0; i < 2.0; i += inv_SR) {
   // std::cout << "Freq @ time " << i << ":  " << b0.get_frequency(i, 24) << std::endl;
    //std::cout << b0.get_frequency(i, 24) << std::endl;
    
    double s = 0.0;
//    m0.frequency(b0.get_frequency(i, 24));
//    m1.frequency(b1.get_frequency(i, 24));
//    m2.frequency(b2.get_frequency(i, 24));
//    m3.frequency(b3.get_frequency(i, 24));
    
    m0.frequency(b0.get_frequency(i, 24));
    std::cout << "freq:  " << b0.get_frequency(i, 24) << std:: endl;
    s = m0.pressure(i, earL) * exp(-4*i);
    //s = m0.pressure(i, earL) + m1.pressure(i, earL) + m2.pressure(i, earL) + m3.pressure(i, earL);

    st.addSample(s, 0);
    st.addSample(s, 1);
  }
  
  st.normalize();
  sfm.open(WriteOnly);
  sfm.writeAudio(st);
  sfm.close();
  
  return 0;
}


