//
//  Triangle.cpp
//  aletler
//
//  Created by Phaedon Sinis on 12/22/13.
//  Copyright (c) 2013 Phaedon Sinis. All rights reserved.
//

#include "Triangle.h"


Vector3d Triangle::normal() const {
  return (b-a).cross(c-a);
}

// area of parallelogram
double Triangle::area_pgram() const {
  return fabs(normal().dot(normal().normalized()));
}

// area of triangle
double Triangle::area() const {
  return 0.5 * area_pgram();
}

Vector3d Triangle::centroid() const {
  return (a + b + c) / 3.0;
}

// Maps point p on the 2d unit triangle ( (0,0), (1,0), (0,1) )
// to a point on this triangle, in 3d
Vector3d Triangle::mapUnitTriangle(const Vector2d &p) const {
  
  // barycentric coords
  double ell1, ell2, ell3;
  ell2 = p.x();
  ell3 = p.y();
  ell1 = 1 - ell2 - ell3;
  
  return (ell1 * a) + (ell2 * b) + (ell3 * c);
  
}

double Triangle::potential(const Vector3d &p) {
  
  
  return fabs (fn_I_qrg(a,b,p) +
               fn_I_qrg(b,c,p) +
               fn_I_qrg(c,a,p));
  /*
   return fabs (goto_I_qrg(a,b,p) +
   goto_I_qrg(b,c,p) +
   goto_I_qrg(c,a,p));
   */
}


double Triangle::integral(scalarFn3d fn,
                const Vector3d &refpt,
                TriangleQuadrature quadtype) const {
  
  
  int quad_num = 0;
  const double *quad_weights;
  const Vector2d *quad_abscissas;
  
  if (quadtype == GAUSS4X4) {
    quad_weights = &gauss4x4_weights[0];
    quad_abscissas = &gauss4x4_abscissas[0];
    quad_num = 16;
  } else if (quadtype == STRANG3) {
    quad_weights = &strang3_weights[0];
    quad_abscissas = &strang3_abscissas[0];
    quad_num = 4;
  } else if (quadtype == STRANG5) {
    quad_weights = &strang5_weights[0];
    quad_abscissas = &strang5_abscissas[0];
    quad_num = 6;
  } else if (quadtype == VERTEX) {
    quad_weights = &vertex_weights[0];
    quad_abscissas = &vertex_abscissas[0];
    quad_num = 3;
    
  } else {
    quad_weights = NULL;
    quad_abscissas = NULL;
  }
  
  double wtdsum = 0;
  
  for (size_t i = 0; i < quad_num; i++) {
    wtdsum += quad_weights[i] * fn(refpt,
                                   mapUnitTriangle(quad_abscissas[i]),
                                   normal());
  }
  
  return area() * wtdsum;
}






double Triangle::goto_I_qrg(const Vector3d &q,
                  const Vector3d &r,
                  const Vector3d &p) {
  
  Vector3d n = ((a-c).cross(b-c)).normalized();
  double h = fabs(n.dot(p-a));
  Vector3d g = p - h * n;
  double sig = ((q-g).cross(r-g)).dot(n);
  
  Vector3d rq = r-q; Vector3d rg = r-g; Vector3d rp = r-p;
  Vector3d qr = q-r; Vector3d qg = q-g; Vector3d qp = q-p;
  
  double N = rq.dot(rg) + rq.norm() * rp.norm();
  double D = -qr.dot(qg) + qr.norm() * qp.norm();
  
  if (N == 0 || D == 0) return 0;
  
  double first_term = (sig / rq.norm()) * log(N / D);
  
  double N2 = sig * rq.dot(rg) * (h - rp.norm());
  double D2 = sig*sig * rp.norm() + h * pow(rq.dot(rg), 2);
  
  double second_term = h * atan2(N2, D2);
  
  double N3 = sig * qr.dot(qg) * (h - qp.norm());
  double D3 = sig*sig * qp.norm() + h * pow(qr.dot(qg), 2);
  
  double third_term = h * atan2(N3, D3);
  
  return first_term + second_term + third_term;
}


// One term in the potential at point P
// H. Wang et al, Harmonic Parameterization by Electrostatics
// (Eqn 15)
double Triangle::fn_I_qrg(const Vector3d &q,
                const Vector3d &r,
                const Vector3d &p) {
  Vector3d n = ((c-a).cross(b-a)).normalized();
  
  double h = n.dot(p-a);
  Vector3d g = p - h * n;
  
  Vector3d rq = q-r;
  Vector3d rg = g-r;
  Vector3d rp = p-r;
  
  Vector3d qr = r-q;
  Vector3d qg = g-q;
  Vector3d qp = p-q;
  
  double sig = (qg.cross(rg)).dot(n);
  double N = rq.dot(rp) + rq.norm() * rp.norm();
  double D = rq.dot(qp) + rq.norm() * qp.norm();
  
  // TODO: check against double precision limits
  if (sig == 0 || N == 0 || D == 0) {
    return 0;
  }
  
  double first_term = (sig * log(N/D)) / rq.norm();
  
  double N2 = sig * rq.dot(rg) * (fabs(h) - rp.norm());
  double D2 = sig*sig * rp.norm() + fabs(h) * pow(rq.dot(rg), 2);
  double second_term = fabs(h) * atan2(N2, D2);
  
  double N3 = sig * qr.dot(qg) * (fabs(h) - qp.norm());
  double D3 = sig*sig * qp.norm() + fabs(h) * pow(qr.dot(qg), 2);
  double third_term = fabs(h) * atan2(N3, D3);
  
  return first_term + second_term + third_term;
}







