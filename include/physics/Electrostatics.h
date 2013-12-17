#ifndef _aletler_electrostatics_h_
#define _aletler_electrostatics_h_

#include <Eigen/Dense>
#include <geometry/TriangleMesh.h>
#include <vector>
#include <numeric/FastMultibubble.h>

using Eigen::MatrixXd;
using Eigen::VectorXd;

class Electrostatics {

 public:
  
  Electrostatics() :
  _bubble(NULL),
  _air(NULL),
  _solid(NULL)
  {}
  
  
  void setBubble(TriangleMesh *b) {
    _bubble = b;
    _nb = _bubble->size();
    computeRHS();
    computeBubbleSubmatrices();

    fmbsolver.setBubbleMatrices(_Abb, _B, _C);
  }
  

  
  void setDomain(TriangleMesh *air, TriangleMesh *solid) {
    
    _air = air;
    _solid = solid;
    
    _na = _air->size();
    _ns = _solid->size();

    precomputeDomainMatrix();
    
    // This is one of the bottlenecks:
    fmbsolver.setDomainMatrix(_D);
  }
  

  double bubbleCapacitance();

  
  double evaluateField(const Vector3d &x) const {

    size_t n = _nb + _na + _ns;
    
    MatrixXd singleLayer(1, n);
    MatrixXd doubleLayer(1, n);
    
    for (size_t i = 0; i < _nb; i++) {
      
      
      Triangle t = _bubble->triangle(i);
      singleLayer(i) = dirichletMatrixElem(t, x);
      doubleLayer(i) = neumannMatrixElem(t, x);
      
    }
    
    return (-doubleLayer * _1_0_0 + singleLayer * _x)(0,0);
  }
  
  
  
  void visualize();
  
  
  
 private:
  
  FastMultibubble fmbsolver;
  
  TriangleMesh *_bubble;
  TriangleMesh *_air;
  TriangleMesh *_solid;
  
  
  // This matrix is the block that represents the fluid/air and fluid/solid matrix elems
  // We will compute it ONCE for each time step and then swap out bubbles
  MatrixXd _D;
  
  // These are the other three blocks, using the notation in James, "Fast Multi-bubble..."
  // where A = [ Abb   B ]
  //           [  C    D ]
  MatrixXd _Abb, _B, _C;
  
  // This is used to compute the RHS
  MatrixXd _Hb;
  
  VectorXd _rhs;
  VectorXd _1_0_0;
  VectorXd _x;

  // number of elements in bubble, air, solid mesh respectively
  size_t _nb, _na, _ns;
  
  
  void precomputeDomainMatrix();


  void computeBubbleSubmatrices();

  
  double dirichletMatrixElem(const Triangle &j, const Vector3d &xi) const {
    return (0.25 * M_1_PI) * j.integral(dirichletMatrixEntry, xi, GAUSS4X4);
  }
  
  double neumannMatrixElem(const Triangle &j, const Vector3d &xi) const {
    return (-0.25 * M_1_PI) * j.integral(neumannMatrixEntry, xi, STRANG3);
  }
  

  

  void computeRHS() {
    std::cout << "nb, na, ns:  " << _nb << " " << _na << " " << _ns << std::endl;
    size_t n = _nb + _na + _ns;
    _Hb.resize(n, _nb);
    
    _1_0_0.resize(_nb);
    _1_0_0.setConstant(1.0);
    
    // Compute this column block of Neumann elements
    for (size_t r = 0; r < n; r++) {
      
      Triangle ti;
      Vector3d cent;
      if (r < _nb) {
        ti = _bubble->triangle(r);
      } else if (r < _nb + _na){
        ti = _air->triangle(r - _nb);
      } else {
        ti = _solid->triangle(r - (_nb + _na));
      }
      cent = ti.centroid();

      
      for (size_t c = 0; c < _nb; c++) {
        
        Triangle tj = _bubble->triangle(c);
        
        _Hb(r, c) = neumannMatrixElem(tj, cent);
        
      }
    }
    
    _rhs = _Hb * _1_0_0;
  }

  

  Triangle triangleAt(size_t i) {
    
    if (i < _nb) {
      return _bubble->triangle(i);
    } else if (i < _nb + _na) {
      return _air->triangle(i - _nb);
    } else if (i < _nb + _na + _ns) {
      return _solid->triangle(i - _nb - _na);
    } else {
      // out of range, throw an error
      std::cout << "NO. This index is invalid. Returning NULL..." << std::endl;
      assert(false);
    }
  }
  
  
  double potentialAt(size_t i) {
    
    if (i < _nb) {
      return 1;
    } else if (i < _nb + _na) {
      return 0;
    } else {
      return -_x(i);
    }
  }
  
  double normalDerivAt(size_t i) {

    if (i < _nb + _na) {
      return _x(i);
    } else {
      return 0.0;
    }
  }

};


#endif
