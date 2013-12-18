//
//  Bubble.cpp
//  aletler
//
//  Created by Phaedon Sinis on 11/29/13.
//  Copyright (c) 2013 Phaedon Sinis. All rights reserved.
//

#include "PhysicalConstants.h"
#include "Bubble.h"
#include <boost/numeric/odeint.hpp>

using namespace boost::numeric::odeint;
typedef boost::array< double , 3 > state_type;

using namespace PhysicalConstants;


static std::vector<double> *g_samples = NULL;
static SoundFrequency *g_soundfreq = NULL;
static double g_r0 = 0;

static double forcingfn(double t) {
  
  // TURN IT OFF
  return 0;
  
  double a = 1.0/10000.0;
  
  // shifted t
  double st = t - a;
  
  return (1.0 / (a * sqrt(M_PI))) * exp(-st*st / (a*a));
}


static double omega(double t) {
  
  return g_soundfreq->frequencyAt(t, FREQ_OMEGA);
}

static void rhs( const state_type &x , state_type &dxdt ,  double t )
{
  
  double omg = omega(t);
  
  double drad = omg * g_r0 / Sound::C_WATER;
  
  double dvis = 4 * Fluids::MU_WATER / (Fluids::RHO_WATER * omg * g_r0*g_r0);
  
  
  
  double psi = (16 * Fluids::THERMAL_DAMPING * Dynamics::G) / (9 * omg * pow(Fluids::GAMMA_AIR - 1, 2));
  
  double dth = 2 * ( sqrt(psi-3) - (3*Fluids::GAMMA_AIR - 1)/(3* (Fluids::GAMMA_AIR - 1))) / (psi - 4);
  
  double delta = drad + dvis + dth;
  
  double damping = omg * delta / sqrt(delta*delta + 4);
  
  dxdt[0] = x[1];
  dxdt[1] = forcingfn(t) - omg * omg * x[0] - damping*x[1];
}

static void store_samples( const state_type &x , const double t ) {
  g_samples->push_back(x[0]);
}


double Bubble::frequency_omega(double capacitance) const {
  
  if (capacitance == 0) return 0;
  
  double four_pi_gamma = 4 * M_PI * PhysicalConstants::Fluids::GAMMA_AIR;
  
  double v = _bubble->volume();
  
  // TODO: doens't take hydrostatic pressure into account
  double p0 = PhysicalConstants::Fluids::P_ATM;
  
  return sqrt( four_pi_gamma * p0 * capacitance / (PhysicalConstants::Fluids::RHO_WATER * v) );
}


void Bubble::integrateVibrationODE() {
  
  // nothing to integrate...
  if (_soundfreq.isEmpty())
    return;
  
  double timeStep = 1.0 / Sound::SAMPLING_RATE;
  
  // temporarily store the address in a global var so the callback can access it
  g_samples = &_samples;
  g_soundfreq = &_soundfreq;
  g_r0 = _r0;
  
  runge_kutta4< state_type > stepper;
  state_type initcondits = { 0.0, -1.0 };
  integrate_const(stepper,
                  rhs,
                  initcondits,
                  _soundfreq.startTime(), _soundfreq.stopTime(),
                  timeStep,
                  store_samples );
  
  // no pointers left behind!
  g_samples = NULL;
  g_soundfreq = NULL;
  
  sample0 = floor(_soundfreq.startTime() * Sound::SAMPLING_RATE);
  samplef = sample0 + _samples.size() - 1;
}


void Bubble::setBubbleMesh(TriangleMesh *b) {
  _bubble = b;
  double bubbleVolume = _bubble->volume();
  double mass_air = bubbleVolume * Fluids::RHO_AIR;
  double mass_water = bubbleVolume * Fluids::RHO_WATER;
  _accel = Vector3d(0, Dynamics::G * (mass_water - mass_air) / (mass_air + mass_water), 0);
  
  if (_r0 < 0) {
    _r0 = pow( 3 * bubbleVolume / (4 * M_PI), 1.0/3.0);
  }
  
}

double Bubble::getSample(size_t sampleIndex) {
  if (_soundfreq.isEmpty()) return 0;
  
  if (sampleIndex < sample0 || sampleIndex > samplef)
    return 0;
  
  size_t arrayIndex = sampleIndex - sample0;
  
  // not sure why this one is also necessary, as I thought I'd caught this case, but
  // I guess there's some edge case:
  if (arrayIndex >= _samples.size()) return 0;
  
  return _samples[arrayIndex];
}