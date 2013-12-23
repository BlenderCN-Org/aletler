//
//  Listener.h
//  soundmath
//
//  Created by Phaedon Sinis on 11/30/12.
//  Copyright (c) 2012 Phaedon Sinis. All rights reserved.
//

#ifndef soundmath_Header_h
#define soundmath_Header_h

#include <vector>

#include <Eigen/Dense>
using namespace Eigen;



class Listener {
    
public:
    Listener() {}
    
    void addLocation(Vector3d loc) {
        m_locations.push_back(loc);
    }
    
    
    // Assume that we're modeling stereo sound
    const Vector3d & leftEar() const { return m_locations[0]; }
    const Vector3d & rightEar() const { return m_locations[1];}
    
private:
    std::vector<Vector3d> m_locations;
    
};

#endif
