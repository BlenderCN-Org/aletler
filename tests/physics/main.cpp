//
//  main.cpp
//  aletler
//
//  Created by Phaedon Sinis on 11/29/13.
//  Copyright (c) 2013 Phaedon Sinis. All rights reserved.
//

#include <physics/Bubble.h>
#include <physics/Electrostatics.h>
#include <physics/PhysicalConstants.h>

#include <iostream>

#include "vdb.h"

#include <sound/Monopole.h>
#include <sound/SoundTrack.h>
#include <sound/SoundFileManager.h>
#include <sound/SoundFrequency.h>

#include <sound/BoundaryPressure.h>

static SoundTrack st(44100, 2);
static SoundFrequency sfvec;


static double omega(double t) {

  return sfvec.frequencyAt(t, FREQ_OMEGA);
}

/******** BOOST STUFF **********/
#include <boost/numeric/odeint.hpp>
using namespace boost::numeric::odeint;
typedef boost::array< double , 3 > state_type;

void rhs( const state_type &x , state_type &dxdt ,  double t )
{
  
  double omg = omega(t);
  
  double drad = omg * 0.005 / PhysicalConstants::Sound::C_WATER;
  
  double dvis = 4 * 0.00089 / (PhysicalConstants::Fluids::RHO_WATER * omg * 0.005*0.005);
  
  
  
  double psi = (16 * 1.6e6 * 9.8) / (9 * omg * (0.4*0.4));

  double dth = 2 * ( sqrt(psi-3) - (3*1.4 - 1)/(3*0.4)) / (psi - 4);

  double delta = drad + dvis + dth;

  double damping = omg * delta / sqrt(delta*delta + 4);
  
  dxdt[0] = x[1];
  dxdt[1] = -omg * omg * x[0] - damping*x[1];
}

void write_cout( const state_type &x , const double t )
{
  
  st.addSample(x[0], 0);
  st.addSample(x[0], 1);
}

void boostmain()
{
  runge_kutta4< state_type > stepper;
  state_type x = { 0.0, 1.0 }; // initial conditions
  integrate_const(stepper, rhs , x , 0.0, 0.5, 1.0/44100.0, write_cout );
}
/******** BOOST STUFF **********/



int main() {

  
  SoundFileManager sfm("/Users/phaedon/bubble_96fps.aiff");
  std::string meshdir = "/Users/phaedon/github/aletler/meshes/";
 
  
  // FIELD EVALUATION HERE
  /*
  MatrixXd fld(21, 1);
  
  for (int i = -10; i <= 10; i++) {
    Vector3d pt(i, 0, 0);
    fld(i + 10) = e.evaluateField(pt);
  }
  
  double fldmin = fld.minCoeff();
  double fldmax = fld.maxCoeff();
  for (int i = -10; i <= 10; i++) {
    Vector3d pt(i, 0, 0);

    double fldval = fld(i + 10);
    std::cout << "field value at x=" << pt.x()  << " :  " << fldval << std::endl;
    double color = (fldval - fldmin) / (fldmax - fldmin);
    vdb_color(0, 0, color);
    vdb_point(pt.x(), pt.y(), pt.z());
  }
  */
  
  
  Bubble b0(0);
  b0.set_directory(meshdir);
 

  b0.compute_all_frequencies(0, 35);
  
  sfvec = b0.soundFrequency();
  
  boostmain();
  st.normalize();
  sfm.open(WriteOnly);
  sfm.writeAudio(st);
  sfm.close();

  
  
  return 0;
}


