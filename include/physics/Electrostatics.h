#ifndef _aletler_electrostatics_h_
#define _aletler_electrostatics_h_

using Eigen::Vector3d;
using Eigen::MatrixXd;

class Electrostatics {

 public:

  // stores vector of capacitances, with indices
  // corresponding to bubble indices
  void capacitance(std::vector<double> &caps) {
    assert(caps.size() == 0 || caps.size() == _bubbles.size());
    
    if (caps.size() == 0) {
      caps.resize(_bubbles.size(), 0);
    }

    for (size_t i = 0; i < _bubbles.size(); i++) {
      caps[i] = single_capacitance(i, true);
    }
  }
  
  void addBubble(TriangleMesh *b) { _bubbles.push_back(b); }
  void setSurface(TriangleMesh *s) { _free_surface = s; }


 private:

  // computes capacitance of a single mesh
  // (generally a bubble) relative to free surface
  double single_capacitance(size_t mesh_index, 
			    bool use_free_surface = true) {
    

    TriangleMesh *currMesh = _bubbles[mesh_index];

    // num triangles in bubble mesh
    size_t bn = _bubbles[mesh_index]->size();

    // num triangles in free-surface meshes
    size_t fsn = 0;
    if (use_free_surface) {
      /*for (size_t i = 0; i < _free_surface.size(); i++) {
	fsn += _free_surface[i]->size();
	}
      */
      fsn = _free_surface->size();
    }

    // total dimension of matrix
    size_t n = bn + fsn;
    MatrixXd A(n, n);
    MatrixXd rhs(n, 1); // 1s and 0s


    // TODO: this is fine for a single bubble, 
    // but over a system of several bubbles we'd
    // be constructing this redundantly
    std::vector<Triangle> tris;
    std::vector<Vector3d> probes;
    MatrixXd inv_area(n, 1);

    // Initialize triangles, probe points, triangle areas
    // for bubble mesh of interest
    for (size_t i = 0; i < bn; ++i) {
      tris.push_back(currMesh->triangle(i));
      probes.push_back(tris[i].centroid());
      inv_area(i) = 1.0 / tris[i].area();
      rhs(i) = 1.0;
    }

    for (size_t i = bn; i < n; ++i) {

      // triangle index
      size_t ti = i - bn;

      tris.push_back(_free_surface->triangle(ti));
      probes.push_back(tris[i].centroid());
      inv_area(i) = 1.0 / tris[i].area();

      // Voltage of triangles at free surface is 0
      rhs(i) = 0.0;
    }

    // Create matrix
    for (size_t p = 0 ; p < n; ++p) {
      for (size_t t = 0; t < n; ++t) {
	A(t, p) = tris[t].potential(probes[p]) * inv_area(t);
      }
    }

    // Solve matrix
    MatrixXd q = A.fullPivLu().solve(rhs);
    
    double C = 0; 
    // Compute solution
    for (size_t i = 0; i < bn; ++i) {
      C += q(i);
    }

    return C;

  }

  std::vector<TriangleMesh *> _bubbles;
  TriangleMesh *_free_surface;

};


#endif
