//
//  FastMultibubble.h
//  aletler
//
//  Created by Phaedon Sinis on 12/15/13.
//  Copyright (c) 2013 Phaedon Sinis. All rights reserved.
//

#ifndef aletler_FastMultibubble_h
#define aletler_FastMultibubble_h

#include <Eigen/Dense>
#include <Eigen/LU>

using Eigen::MatrixXd;
using Eigen::VectorXd;
using Eigen::FullPivLU;

enum FastMBBlock {FMB_Abb, FMB_B, FMB_C, FMB_D};

class FastMultibubble {
public:
  
  // assumes that matrix m is the full size of A = [A_bb B  | C D] but that
  // D has not necessarily been set
  // nb is the number of mesh elements that comprise the bubble. Note that
  // this is the number of rows and columns of A_bb
  void setBubbleMatrices(const MatrixXd &m, size_t nb) {
    size_t n = m.rows();
    size_t nas = n - nb;
    
    // top left
    setMatrixBlock(m.block(0, 0, nb, nb), FMB_Abb);
    
    // top right
    setMatrixBlock(m.block(0, nb, nb, nas), FMB_B);
    
    // bottom left
    setMatrixBlock(m.block(nb, 0, nas, nb), FMB_C);
  }
  
  
  // this assumes that matrix m is the specific block you want to set
  void setDomainMatrix(const MatrixXd &m) {
    setMatrixBlock(m, FMB_D);
  }
  

  void solve(const VectorXd &rhs, VectorXd &x) {
    // It's ok to call this here, because we'll only really need to
    // solve one linear equation per bubble:
    invertFullMatrix();
    x = _A_inv * rhs;
  }
  
  void solve_slow(const VectorXd &rhs, VectorXd &x) {
   // std::cout << _A.rows() << "  x  " << _A.cols()  << std::endl;
    x = _A.fullPivLu().solve(rhs);
  }

  // separate function so we can have accurate timing on the solves:
  void assembleA() {
    _A.resize(_Abb.rows() + _C.rows(), _Abb.cols() + _B.cols());
    
    _A.block(0, 0, _Abb.rows(), _Abb.cols()) = _Abb;
    _A.block(0, _Abb.cols(), _B.rows(), _B.cols()) = _B;
    _A.block(_Abb.rows(), 0, _C.rows(), _C.cols()) = _C;
    _A.block(_Abb.rows(), _Abb.cols(), _D.rows(), _D.cols()) = _D;
  }
  

  
private:
  
  // Ax = b is the linear system we want to solve.
  // Then using the notation from James 2013 ("Fast Multi-bubble...")
  // A = [ A_bb B | C D ]
  // A^-1 = [ X Y | Z U ]
  // Note that since all we need is A^-1, and we have a fast way to compute it
  // using the cached inverse of its component blocks, we never actually assemble
  // matrix A.
  

  MatrixXd _A_inv;

  MatrixXd _Abb, _B, _C, _D;
  MatrixXd _D_inv;
  MatrixXd _X, _Y, _Z, _U;
  
  // Here's the fully assembled matrix A. ONLY to be used for testing speed!!!
  MatrixXd _A;
  
  
  
  
  // Assumes that we've already computed & stored the inverse of the large block (D)
  void invertFullMatrix() {
    
    MatrixXd M1 = _B * _D_inv;
    MatrixXd M2 = M1 * _C;
    
    _X = (_Abb - M2).inverse();
    _Y = -_X * M1;
    
    MatrixXd M3 = _C * _Abb.inverse();
    _U = (_D - M3 * _B).inverse();
    _Z = -_U * M3;
    
    // Use block assignment to merge all of these elements into an inverse:
    _A_inv.resize(_X.rows() + _Z.rows(), _X.cols() + _Y.cols());
    
    _A_inv.block(0, 0, _X.rows(), _X.cols()) = _X;
    _A_inv.block(0, _X.cols(), _Y.rows(), _Y.cols()) = _Y;
    _A_inv.block(_X.rows(), 0, _Z.rows(), _Z.cols()) = _Z;
    _A_inv.block(_X.rows(), _X.cols(), _U.rows(), _U.cols()) = _U;
  }
  
  
  
  void invertD() {
    
    // use LU factorization:
    FullPivLU<MatrixXd> luOfD(_D);
    if (! luOfD.isInvertible()) {
      std::cout << "FastMultibubble solver message:  Matrix D is not invertible. Exiting..." << std:: endl;
      assert(false);
    }
    
    // and store the inverse.
    _D_inv = luOfD.inverse();
  }
  

  
  
  // utility function for setting the various blocks
  void setMatrixBlock(const MatrixXd &m, FastMBBlock block) {
    switch (block) {
      case FMB_Abb:
        _Abb = m;
        break;
      case FMB_B:
        _B = m;
        break;
      case FMB_C:
        _C = m;
        break;
      case FMB_D:
        _D = m;
        invertD(); // critical step!
        break;
        
      default:
        assert(false);
        break;
    }
  }
  
};


#endif
