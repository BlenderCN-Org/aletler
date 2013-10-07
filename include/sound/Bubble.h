//
//  Bubble.h
//  soundmath
//
//  Created by Phaedon Sinis on 11/30/12.
//  Copyright (c) 2012 Phaedon Sinis. All rights reserved.
//

#ifndef soundmath_Bubble_h
#define soundmath_Bubble_h


#include "Monopole.h"

class Bubble {
    
public:
    Bubble(double mmradius, double btime, Vector3d loc) {
        
        m_freq = minnaert(mmradius);
        
        m_mpole.frequency(m_freq);
        m_mpole.position(loc);
        
        m_birthtime = btime;
    }
    
    double minnaert(double mmradius) {
        
        m_radius = mmradius / 1000.0;
        
        double gamma = 1.4;
        double rho = 1000;
        double p0 = 101325;
        
        return (0.5 / M_PI) * (1.0 / m_radius) * sqrt(3 * gamma * p0 / rho);
        
    }
    
    double pressure(double t, Vector3d ear) {
        if (t < m_birthtime || t > m_birthtime + S_BUBBLELIFE) return 0;
        
        double t_age = t - m_birthtime;
        
        m_mpole.frequency(m_freq * exp(4 * (t_age)));
        
        double attack = 2 * atan(t_age * 0.5) * M_1_PI;
        double attenuation = exp(-50 * t_age);
        
        return attack * attenuation * m_mpole.pressure(t_age, ear);
    }
    
    // accessors
    double get_birthtime() const {
        return m_birthtime;
    }
    
    // predicate function for sorting bubbles
    static
    bool sort_by_birth(const Bubble* a, const Bubble* b) {
        return a->get_birthtime() < b->get_birthtime();
    }
    
    // TODO: generalize. Currently assumes 100ms for bubble duration
    static const double S_BUBBLELIFE;
    
private:
    double m_radius;
    double m_freq;
    
    Monopole m_mpole;
    
    double m_birthtime;
    

    
};

const double Bubble::S_BUBBLELIFE = 0.1;

#endif
