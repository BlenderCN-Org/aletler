//
//  SoundParameters.h
//  soundmath
//
//  Created by Katherine Breeden on 12/6/12.
//

#ifndef soundmath_SoundParameters_h
#define soundmath_SoundParameters_h

// an epsilon for assertion testing
double VERY_SMALL = 0.000001;


// constants related to physical parameters
static const double RHO_AIR = 1.2;
static const double RHO_WATER = 1000.0;

static const double C_AIR = 344.0;
static const double C_WATER = 1480.0;



// simulation details
static const int SAMPLING_RATE = 44100;


// HRTF consts
static const double HEAD_WIDTH = 0.15;

#endif
