//
//  Triangle.h
//  aletler
//
//  Created by Phaedon Sinis on 12/22/13.
//  Copyright (c) 2013 Phaedon Sinis. All rights reserved.
//

#ifndef __aletler__Triangle__
#define __aletler__Triangle__


#include "TriangleQuadrature.h"
#include <Eigen/Dense>

using Eigen::Vector3d;

typedef double (*scalarFn3d)(const Vector3d &yi, const Vector3d &xj, const Vector3d &nj);

enum FluidBoundaryType {
  FBT_BUBBLE,
  FBT_AIR,
  FBT_SOLID
};

enum TriangleQuadrature {
  VERTEX,
  STRANG3,
  STRANG5,
  GAUSS4X4,
};


// A Triangle actually contains 3 points and can stand
// on its own (outside of a TriangleMesh object)
class Triangle {
public:
  
  // Assume counterclockwise winding order
  Vector3d a, b, c;
  
  FluidBoundaryType boundaryType;
  
  Vector3d normal() const;
  
  // area of parallelogram and triangle
  double area_pgram() const;
  double area() const;
  
  
  double potential(const Vector3d &p);
  
  Vector3d centroid() const;
  

  
  double integral(scalarFn3d fn,
                  const Vector3d &refpt,
                  TriangleQuadrature quadtype = GAUSS4X4) const;
  
  
  
private:
  
  
  // Maps point p on the 2d unit triangle ( (0,0), (1,0), (0,1) )
  // to a point on this triangle, in 3d
  Vector3d mapUnitTriangle(const Vector2d &p) const;

  
  // One term in the potential at point P
  // H. Wang et al, Harmonic Parameterization by Electrostatics
  // (Eqn 15)
  double fn_I_qrg(const Vector3d &q,
                  const Vector3d &r,
                  const Vector3d &p);
  
  // Same as fn_I_qrg, but using the notation in the original Goto paper
  // (not used)
  double goto_I_qrg(const Vector3d &q,
                    const Vector3d &r,
                    const Vector3d &p);
  
  
};


#endif /* defined(__aletler__Triangle__) */
