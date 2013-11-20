#ifndef _ALETLER_BOUNDINGBOX_H_
#define _ALETLER_BOUNDINGBOX_H_

#include <iostream>
#include <Eigen/Dense>

using Eigen::Vector3d;

class BoundingBox {

 public:

  BoundingBox(Vector3d &bmin, Vector3d &bmax);

  BoundingBox(double x0, double y0, double z0,
	      double xf, double yf, double zf);

  // Returns true if the box contains point p.
  bool contains(const Vector3d &p);


 private:
  Vector3d boxmin;
  Vector3d boxmax;



};


#endif
