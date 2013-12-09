#ifndef _aletler_electrostatics_h_
#define _aletler_electrostatics_h_

#include <Eigen/Dense>
#include <geometry/TriangleMesh.h>
#include <vector>

using Eigen::MatrixXd;


class Electrostatics {

 public:
  
  // stores vector of capacitances, with indices
  // corresponding to bubble indices
  void capacitance(std::vector<double> &caps);
  void addBubble(TriangleMesh *b) { _bubbles.push_back(b); }
  void setBubble(TriangleMesh *b) { _bubble = b; }
  void setSurface(TriangleMesh *s) { _free_surface = s; }
  void setSolid(TriangleMesh *s) { _solid = s; }
  
  void computeDirichletMatrix() {
    size_t n = _bubble->size() + _free_surface->size() + _solid->size();
    size_t n_bub_fs = _bubble->size() + _free_surface->size();
    _dirichletMatrix.resize(n, n);
    _dirichletMatrix.setZero();
    
    for (size_t i = 0; i < n; i++) {
      
      // because the last column is zeroed out in the multiplication
      // Hp = Gt where G is the dirichlet matrix, we don't bother
      for (size_t j = 0; j < n_bub_fs; j++) {
        
        Triangle ti = triangleAt(i);
        Triangle tj = triangleAt(j);
        
        Vector3d ci = ti.centroid();
        _dirichletMatrix(i, j) = tj.potential(ci) / tj.area();
        
      }
    }
  }
  
  
  double neumannMatrixElem(const Triangle &j, const Vector3d &xi) {
    
    return (-0.25 * M_1_PI) * j.integral(neumannMatrixEntry, xi, STRANG3);
    
  }
  
  void computeNeumannMatrix() {
    
    size_t nb = _bubble->size();
    size_t nf = _free_surface->size();
    
    size_t n = _bubble->size() + _free_surface->size() + _solid->size();
    _neumannMatrix.resize(n, n);
    _neumannMatrix.setZero();
    
    for (size_t i = 0; i < n; i++) {
      for (size_t j = 0; j < n; j++) {
        
        // only set non-diagonal entries:
        if (i != j) {
          
          // and only for the H_B and H_S columns (H_F is multiplied by zeros in the special case of
          // the free surface, so we leave it out to avoid integrating each of those triangles)
          if (j < nb || j >= (nb + nf)) {
            Triangle ti = triangleAt(i);
            Triangle tj = triangleAt(j);
        
            Vector3d ci = ti.centroid();
          
            _neumannMatrix(i, j) = neumannMatrixElem(tj, ci);
          }
          
          
        } else {
          
          // set all the diagonals to 1/2
          _neumannMatrix(i, i) = 0.5;
        }
        
      }
    } // end for-loops
    
    computeRHS();
    
  }
  
  void computeCombinedMatrix() {
    
    size_t n = _bubble->size() + _free_surface->size() + _solid->size();
    size_t n_bub_fs = _bubble->size() + _free_surface->size();
    
    _combinedMatrix.resize(n, n);
    _combinedMatrix.setZero();
    
    
    for (size_t i = 0; i < n; i++) {
      for (size_t j = 0; j < n; j++) {
        
        if (j < n_bub_fs) {
          _combinedMatrix(i, j) = _dirichletMatrix(i, j);
        } else {
          _combinedMatrix(i, j) = _neumannMatrix(i, j);
        }
        
      }
    }
    
    
  }
  
  void solveLinearSystem() {
    _q = _combinedMatrix.fullPivLu().solve(_rhs);
  }
  
  double bubbleCapacitance() {
    double c = 0;
    for (size_t i = 0; i < _bubble->size(); i++) {
      c += _q(i);
    }
    return c;
  }
  
  
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

  
 private:
    
  // MUST be called after Neumann matrix has been computed
  void computeRHS() {
    
    size_t nb = _bubble->size();
    size_t nf = _free_surface->size();
    size_t ns = _solid->size();
    
    _1_0_0.resize(nb+nf+ns, 1);
    _1_0_0.setZero();
    
    for (size_t i = 0; i < nb; i++) {
      _1_0_0(i) = 1.0;
    }
    
    _rhs = _neumannMatrix * _1_0_0;
    //_rhs = _1_0_0;
  }

  double single_capacitance(size_t mesh_index, bool use_free_surface = true);

  MatrixXd _dirichletMatrix;
  MatrixXd _neumannMatrix;
  MatrixXd _combinedMatrix;
  MatrixXd _rhs;
  
  MatrixXd _1_0_0;
  
  MatrixXd _q;
  
  std::vector<TriangleMesh *> _bubbles;
  TriangleMesh *_bubble;
  TriangleMesh *_free_surface;
  TriangleMesh *_solid;
};


#endif
