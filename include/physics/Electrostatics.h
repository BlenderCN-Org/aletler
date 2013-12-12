#ifndef _aletler_electrostatics_h_
#define _aletler_electrostatics_h_

#include <Eigen/Dense>
#include <geometry/TriangleMesh.h>
#include <vector>

using Eigen::MatrixXd;
using Eigen::VectorXd;

class Electrostatics {

 public:
  
  void setBubble(TriangleMesh *b) { _bubble = b; }
  void setSurface(TriangleMesh *s) { _free_surface = s; }
  void setSolid(TriangleMesh *s) { _solid = s; }
  
  
  void computeDirichletMatrix();
  void computeNeumannMatrix();
  void solveLinearSystem();
  double bubbleCapacitance();

  
  double evaluateField(const Vector3d &x) const {
    size_t nb = _bubble->size();
    size_t nf = _free_surface->size();
    size_t ns = _solid->size();
    size_t n = nb + nf + ns;
    
    MatrixXd singleLayer(1, n);
    MatrixXd doubleLayer(1, n);
    
    for (size_t i = 0; i < nb; i++) {
      
      
      Triangle t = _bubble->triangle(i);
      singleLayer(i) = dirichletMatrixElem(t, x);
      doubleLayer(i) = neumannMatrixElem(t, x);
      
    }
    
    return (-doubleLayer * _1_0_0 + singleLayer * _q)(0,0);
  }
  
  
  
 private:
  
  MatrixXd _dirichletMatrix;
  MatrixXd _neumannMatrix;
  MatrixXd _combinedMatrix;
  
  VectorXd _rhs;
  VectorXd _1_0_0;
  VectorXd _q;

  
  
  

  
  double dirichletMatrixElem(const Triangle &j, const Vector3d &xi) const {
    return (0.25 * M_1_PI) * j.integral(dirichletMatrixEntry, xi, GAUSS4X4);
  }
  
  double neumannMatrixElem(const Triangle &j, const Vector3d &xi) const {
    return (-0.25 * M_1_PI) * j.integral(neumannMatrixEntry, xi, STRANG3);
  }
  
  
  
  // MUST be called after Neumann matrix has been computed
  void computeRHS() {
    
    size_t nb = _bubble->size();
    size_t nf = _free_surface->size();
    size_t ns = _solid->size();
    
    _1_0_0.resize(nb+nf+ns);
    _1_0_0.setZero();
    _1_0_0.head(nb).setConstant(1.0);
    
    _rhs = _neumannMatrix * _1_0_0;
  }

  
  void computeCombinedMatrix();
  void precomputeTriangles();

  
  // precomputed stuff
  std::vector<Vector3d> _centroids;
  std::vector<Triangle> _triangles;
  
  TriangleMesh *_bubble;
  TriangleMesh *_free_surface;
  TriangleMesh *_solid;
  
  
  Triangle triangleAt(size_t i) {
    
    size_t nb = _bubble->size();
    size_t nf = _free_surface->size();
    size_t ns = _solid->size();
    
    if (i < nb) {
      return _bubble->triangle(i);
    } else if (i < nb + nf) {
      return _free_surface->triangle(i - nb);
    } else if (i < nb + nf + ns) {
      return _solid->triangle(i - nb - nf);
    } else {
      // out of range, throw an error
      std::cout << "NO. This index is invalid. Returning NULL..." << std::endl;
      assert(false);
    }
  }
  
  
  double potentialAt(size_t i) {
    
    size_t nb = _bubble->size();
    size_t nf = _free_surface->size();
    
    if (i < nb) {
      return 1;
    } else if (i < nb + nf) {
      return 0;
    } else {
      return -_q(i);
    }
  }
  
  double normalDerivAt(size_t i) {
    size_t nb = _bubble->size();
    size_t nf = _free_surface->size();
    
    if (i < nb + nf) {
      return _q(i);
    } else {
      return 0.0;
    }
  }

};


#endif
