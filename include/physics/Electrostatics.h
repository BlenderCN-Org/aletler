#ifndef _aletler_electrostatics_h_
#define _aletler_electrostatics_h_

#include <Eigen/Dense>
#include <geometry/TriangleMesh.h>
#include <vector>



class Electrostatics {

 public:

  // stores vector of capacitances, with indices
  // corresponding to bubble indices
  void capacitance(std::vector<double> &caps);
  void addBubble(TriangleMesh *b) { _bubbles.push_back(b); }
  void setSurface(TriangleMesh *s) { _free_surface = s; }


 private:

  double single_capacitance(size_t mesh_index, bool use_free_surface = true);

  std::vector<TriangleMesh *> _bubbles;
  TriangleMesh *_free_surface;

};


#endif
