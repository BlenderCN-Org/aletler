#ifndef _aletler_physicalconstants_h_
#define _aletler_physicalconstants_h_

namespace PhysicalConstants {

  namespace Fluids {
    static const double RHO_AIR = 1.2;
    static const double RHO_WATER = 1000.0;

    // Surface tension of air in water                                                  
    static const double SIGMA_AIR_WATER = 0.073;

    // heat capacity ratio, or adiabatic gas constant                                   
    // "gamma > 1 for any ideal gas"                                                    
    static const double GAMMA_AIR = 1.4;

    // 1 ATM pressure, in Pascals:                                                      
    static const double P_ATM = 101325;

    // Molecular weight
    static const double MW_AIR = 29;

    // Ideal gas constant
    static const double IDEALGAS_AIR = 8314.5;

    // 0C in Kelvin:
    static const double KELVIN = 273.15;
    
    // Shear viscosity of water:
    static const double MU_WATER = 0.00089;
    
    static const double THERMAL_DAMPING = 1.6e6;
  }

  namespace Sound {
    // speed of sound
    static const double C_AIR = 344.0;
    static const double C_WATER = 1480.0;

    static const int SAMPLING_RATE = 44100;

    // for HRTF use
    static const double HEAD_WIDTH = 0.15;
  }

  namespace Dynamics {
    static const double G = 9.80665;
  }

};

#endif
