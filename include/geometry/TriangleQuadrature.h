//
//  TriangleQuadrature.h
//  aletler
//
//  Created by Phaedon Sinis on 12/22/13.
//  Copyright (c) 2013 Phaedon Sinis. All rights reserved.
//

#ifndef aletler_TriangleQuadrature_h
#define aletler_TriangleQuadrature_h

#include <Eigen/Dense>

using Eigen::Vector2d;

// VERTEX, order 3, degree precision 1
static const Vector2d vertex_abscissas[3] = {
  Vector2d(1.00000000000000000000,  0.00000000000000000000),
  Vector2d(0.00000000000000000000,  1.00000000000000000000),
  Vector2d(0.00000000000000000000,  0.00000000000000000000)
};


static const double vertex_weights[3] = {
  0.33333333333333333333,
  0.33333333333333333333,
  0.33333333333333333333
};


// STRANG3, order 4, degree of precision 3
static const Vector2d strang3_abscissas[4] = {
  Vector2d(0.33333333333333333333,  0.33333333333333333333),
  Vector2d(0.60000000000000000000,  0.20000000000000000000),
  Vector2d(0.20000000000000000000,  0.60000000000000000000),
  Vector2d(0.20000000000000000000,  0.20000000000000000000)
};

static const double strang3_weights[4] = {
  -0.56250000000000000000,
  0.52083333333333333333,
  0.52083333333333333333,
  0.52083333333333333333
};

// STRANG5, order 6, dop 4
static const Vector2d strang5_abscissas[6] = {
  Vector2d(0.816847572980459,  0.091576213509771),
  Vector2d(0.091576213509771,  0.816847572980459),
  Vector2d(0.091576213509771,  0.091576213509771),
  Vector2d(0.108103018168070,  0.445948490915965),
  Vector2d(0.445948490915965,  0.108103018168070),
  Vector2d(0.445948490915965,  0.445948490915965)
};

static const double strang5_weights[6] = {
  0.109951743655322,
  0.109951743655322,
  0.109951743655322,
  0.223381589678011,
  0.223381589678011,
  0.223381589678011
};


// http://people.sc.fsu.edu/~%20jburkardt/datasets/quadrature_rules_tri/quadrature_rules_tri.html
// GAUSS4X4, order 16, degree of precision 7, (essentially a product of two 4 point 1D Gauss-Legendre rules).
static const Vector2d gauss4x4_abscissas[16] = {
  Vector2d(0.0571041961,  0.06546699455602246),
  Vector2d(0.2768430136 , 0.05021012321401679),
  Vector2d(0.5835904324 , 0.02891208422223085),
  Vector2d(0.8602401357 , 0.009703785123906346),
  Vector2d(0.0571041961 , 0.3111645522491480),
  Vector2d(0.2768430136 , 0.2386486597440242),
  Vector2d(0.5835904324 , 0.1374191041243166),
  Vector2d(0.8602401357 , 0.04612207989200404),
  Vector2d(0.0571041961 , 0.6317312516508520),
  Vector2d(0.2768430136 , 0.4845083266559759),
  Vector2d(0.5835904324 , 0.2789904634756834),
  Vector2d(0.8602401357 , 0.09363778440799593),
  Vector2d(0.0571041961 , 0.8774288093439775),
  Vector2d(0.2768430136 , 0.6729468631859832),
  Vector2d(0.5835904324 , 0.3874974833777692),
  Vector2d(0.8602401357 , 0.1300560791760936)};

static const double gauss4x4_weights[16] = {
  0.04713673637581137,
  0.07077613579259895,
  0.04516809856187617,
  0.01084645180365496,
  0.08837017702418863,
  0.1326884322074010,
  0.08467944903812383,
  0.02033451909634504,
  0.08837017702418863,
  0.1326884322074010,
  0.08467944903812383,
  0.02033451909634504,
  0.04713673637581137,
  0.07077613579259895,
  0.04516809856187617,
  0.01084645180365496};




#endif
