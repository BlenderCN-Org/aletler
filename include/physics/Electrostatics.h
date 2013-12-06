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
  void setSurface(TriangleMesh *s) { _free_surface = s; }

  
  
  double neumannMatrixElem(const Triangle &j, const Vector3d &xi) {
    
    return (-0.25 * M_1_PI) * j.integral(neumannMatrixEntry, xi);
    
  }
  
  void computeNeumannMatrix() {
    
    size_t n = _bubble->size() + _free_surface->size() + _solid->size();
    _neumannMatrix.resize(n, n);
    _neumannMatrix.setZero();
    
    for (size_t i = 0; i < n; i++) {
      for (size_t j = 0; j < n; j++) {
        
        // only set non-diagonal entries:
        if (i != j) {
          Triangle ti = triangleAt(i);
          Triangle tj = triangleAt(j);
        
          Vector3d ci = ti.centroid();
          
          _neumannMatrix(i, j) = neumannMatrixElem(tj, ci);
        }
        
      }
    } // end for-loops
    
  }
  
  void computeRHS() {
    size_t nb = _bubble->size();
    size_t nf = _free_surface->size();
    size_t ns = _solid->size();
    
    MatrixXd _1_0_0(nb+nf+ns, 1);
    _1_0_0.setZero();
    for (size_t i = 0; i < nb; i++)
      _1_0_0(i) = 1.0;
    
    _rhs = _neumannMatrix * _1_0_0;
  }
  
 private:
  
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

  double single_capacitance(size_t mesh_index, bool use_free_surface = true);

  MatrixXd _dirichletMatrix;
  MatrixXd _neumannMatrix;
  MatrixXd _rhs;
  
  std::vector<TriangleMesh *> _bubbles;
  TriangleMesh *_bubble;
  TriangleMesh *_free_surface;
  TriangleMesh *_solid;
};


#endif
