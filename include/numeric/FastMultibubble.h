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
  
  void setBubbleMatrices(const MatrixXd &a,
                         const MatrixXd &b,
                         const MatrixXd &c) {
    setMatrixBlock(a, FMB_Abb);
    setMatrixBlock(b, FMB_B);
    setMatrixBlock(c, FMB_C);

  }
  
  
  // this assumes that matrix m is the specific block you want to set
  void setDomainMatrix(const MatrixXd &m) {
    setMatrixBlock(m, FMB_D);
  }
  

  void solve(const VectorXd &rhs, VectorXd &x) {
    // It's ok to call this here, because we'll only really need to
    // solve one linear equation per bubble:
    
    fastInverseSetup();
    
    x.resize(_Abb.rows() + _D.rows(), 1);
    
    VectorXd bb = rhs.block(0, 0, _Abb.rows(), 1);
    VectorXd bas = rhs.block(_Abb.rows(), 0, _D.rows(), 1);
    
    VectorXd xb(_Abb.rows());
    VectorXd xas(_D.rows());
    
    VectorXd t1 = _D_LU.solve(bas);
    VectorXd t2 = bb - _B * t1;
    xb = _X_LU.solve(t2);
    
    
    VectorXd tt1 = _Abb_LU.solve(bb);
    VectorXd tt2 = bas - _C * tt1;
    VectorXd y1 = _D_LU.solve(tt2);
    VectorXd y2 = -_B * y1;
    VectorXd tt3 = _X_LU.solve(y2);
    
    VectorXd tt4 = _C * tt3;
    VectorXd tt5 = tt2 - tt4;
    xas = _D_LU.solve(tt5);
    
    x.block(0, 0, _Abb.rows(), 1) = xb;
    x.block(_Abb.rows(), 0, _D.rows(), 1) = xas;
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
  
  MatrixXd _Abb, _B, _C, _D;
  MatrixXd _Y;
  
  // Here's the fully assembled matrix A. ONLY to be used for testing speed!!!
  MatrixXd _A;
  
  
  // use LU factorization:
  FullPivLU<MatrixXd> _Abb_LU;
  FullPivLU<MatrixXd> _D_LU;
  FullPivLU<MatrixXd> _X_LU;
  
  void fastInverseSetup() {
    
    

    _Abb_LU = FullPivLU<MatrixXd>(_Abb);

    MatrixXd T1 = _D_LU.solve(_C);
    _X_LU = FullPivLU<MatrixXd>(_Abb - _B * T1);
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
        _D_LU = FullPivLU<MatrixXd>(m);
        break;
        
      default:
        assert(false);
        break;
    }
  }
  
};


#endif
