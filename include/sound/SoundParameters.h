//
//  SoundParameters.h
//  soundmath
//
//  Created by Katherine Breeden on 12/6/12.
//

#ifndef soundmath_SoundParameters_h
#define soundmath_SoundParameters_h

// an epsilon for assertion testing
static const double VERY_SMALL = 0.000001;


// constants related to physical parameters
static const double RHO_AIR = 1.2;
static const double RHO_WATER = 1000.0;

static const double C_AIR = 344.0;
static const double C_WATER = 1480.0;

// 1 ATM pressure, in Pascals:
static const double P_ATM = 101325;

static const double GRAVITY = 9.80665;

// Surface tension of air in water
static const double SIGMA_AIR_WATER = 0.073;

// heat capacity ratio, or adiabatic gas constant
// "gamma > 1 for any ideal gas"
static const double GAMMA_AIR = 1.4;


// simulation details
static const int SAMPLING_RATE = 44100;


// HRTF consts
static const double HEAD_WIDTH = 0.15;

#endif
