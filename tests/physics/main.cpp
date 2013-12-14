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

static SoundTrack st(44100, 2);
static SoundFrequency sfvec;

static double freqvector[24] = {
  486.71,
  487.995,
  495.348,
  506.426,
  516.682,
  525.248,
  532.93,
  540.63,
  548.971,
  558.292,
  568.727,
  580.268,
  592.803,
  606.146,
  620.053,
  634.26,
  648.553,
  662.924,
  677.844,
  694.717,
  716.486,
  747.851,
  791.852,
  838.173};

static double omega(double t) {
  //double f0 = 440;
  //double f1 = 660;
  //double a = t;
  //return (f0 * (1-a) + /f1 * a) * 2 * M_PI;
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
  integrate_const(stepper, rhs , x , 0.0, 0.250, 1.0/44100.0, write_cout );
}
/******** BOOST STUFF **********/




int main() {

  // read in the frequency vector
  for (int i = 0; i < 24; i++) {
    sfvec.addFrequency(i * (1.0/150), freqvector[i], FREQ_HERTZ);
  }
  
  SoundFileManager sfm("/Users/phaedon/bubbles.aiff");
  boostmain();
  st.normalize();
  sfm.open(WriteOnly);
  sfm.writeAudio(st);
  sfm.close();
  return 0;
  
  std::string bubbledir = "/Users/phaedon/github/bem-laplace-simple/meshes/";
  std::string meshdir = "/Users/phaedon/github/aletler/meshes/";
  
  
  TriangleMesh bubblemesh, freesurfacemesh, solidsurfacemesh;
  
  bubblemesh.read(meshdir + "rising_bubble_000001.obj", MFF_OBJ);
  freesurfacemesh.read(meshdir + "free_surface_glass.obj", MFF_OBJ);
  solidsurfacemesh.read(meshdir + "solid_glass.obj", MFF_OBJ);

  
  
  Electrostatics e;
  e.setBubble(&bubblemesh);
  e.setSurface(&freesurfacemesh);
  e.setSolid(&solidsurfacemesh);
  
  bubblemesh.flipNormals();
  
  /*
  std::cout << "computing dirichlet matrix" << std::endl;
  e.computeDirichletMatrix();
  
  std::cout << "computing neumann matrix" << std::endl;
  e.computeNeumannMatrix();
  
  std::cout << "solving linear system" << std::endl;
  e.solveLinearSystem();
  
  std::cout << "bubble capacitance!    " << e.bubbleCapacitance() << std::endl;
*/
  
  
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
  
  
  
  std::cout << "done!" << std::endl;
  //return 1;
  
  
  
  Bubble b0(0);

  b0.set_directory(meshdir);
 

  b0.compute_all_frequencies(0, 24);
  // b1.compute_all_frequencies(0, 48);
  //b2.compute_all_frequencies(0, 48);
  //b3.compute_all_frequencies(0, 48);

  
  for (size_t i = 0; i < 24; i++) {
    std::cout << "Freq: "
    << b0.get_frequency(i) << "  "
    //<< b1.get_frequency(i) << "  "
    //<< b2.get_frequency(i) << "  "
    //<< b3.get_frequency(i) << "  "
    << std::endl;
  }
  
  
  
  /*
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
  */
  
  
  return 0;
}


