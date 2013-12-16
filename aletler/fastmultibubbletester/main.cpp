//
//  main.cpp
//  fastmultibubbletester
//
//  Created by Phaedon Sinis on 12/15/13.
//  Copyright (c) 2013 Phaedon Sinis. All rights reserved.
//

#include <iostream>
#include <numeric/FastMultibubble.h>
#include <sound/Timer.h>

using Eigen::MatrixXd;

static Eigen::MatrixXd d;
static FastMultibubble fm;


void testSolverSpeed(size_t nb, size_t nas, bool recomputeDomain) {
  
  std::cout << "SIZE OF BUBBLE MESH:  " << nb << std:: endl;
  std::cout << "SIZE OF AIR & SURFACE BOUNDARIES:  " << nas << std:: endl;

  Timer t;
  
  size_t n = nb + nas;
  
  if (recomputeDomain) {
    MatrixXd d_rand = Eigen::MatrixXd::Random(nas, nas);
    d = d_rand * d_rand.transpose();
  }

  
  MatrixXd a(n, n);
  MatrixXd mbub_rand = Eigen::MatrixXd::Random(nb, nb);
  Eigen::MatrixXd mbub = mbub_rand * mbub_rand.transpose();
  a.block(0, 0, nb, nb) = mbub;
  
  a.block(0, nb, nb, nas) = Eigen::MatrixXd::Random(nb, nas);
  a.block(nb, 0, nas, nb) = Eigen::MatrixXd::Random(nas, nb);
  
  Eigen::VectorXd rhs = Eigen::VectorXd::Random(n);
  Eigen::VectorXd x(n);
  Eigen::VectorXd xslow(n);

  fm.setBubbleMatrices(a, nb);
  
  if (recomputeDomain) {
    t.start_timer("Inverting domain matrix");
    fm.setDomainMatrix(d);
    t.stop_timer();
  }

  
  fm.assembleA();
 
  /*
 // std::cout << "slow solve!" << std::endl;
  t.start_timer("Slow solver");
  fm.solve_slow(rhs, xslow);
  t.stop_timer();
  */
  
  
  t.start_timer("Fast solver");

  //std::cout << "fast solve!" << std::endl;
  fm.solve(rhs, x);
  t.stop_timer();
  
  Eigen::VectorXd xdiff = x - xslow;
  
  std::cout << "x min & max: " << x.minCoeff() << "    " << x.maxCoeff() << std::endl;
  std::cout << "diff min & max:  " << xdiff.minCoeff() << "    " << xdiff.maxCoeff() << std::endl;
  
  std::cout << "\n\n" << std::endl;
}


int main(int argc, const char * argv[])
{
  Timer t;

  for (int s = 4000; s <= 4000; s += 1000) {
    
    bool firstDomainTest = true;

    for (int b = 800; b <= 1000; b += 100) {
      testSolverSpeed(b, s, firstDomainTest);
      firstDomainTest = false;
    }
  }
  return 0;
}




