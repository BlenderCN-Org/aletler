#include "BoundingBox.h"

BoundingBox::BoundingBox(Vector3d &bmin, Vector3d &bmax) 
  : boxmin(bmin),
    boxmax(bmax) {
    }

BoundingBox::BoundingBox(double x0, double y0, double z0,
			 double xf, double yf, double zf) {
  boxmin[0] = x0;
  boxmin[1] = y0;
  boxmin[2] = z0;
  boxmax[0] = xf;
  boxmax[1] = yf;
  boxmax[2] = zf;
}

  // Returns true if the box contains point p.
bool BoundingBox::contains(const Vector3d &p) {
    
  return (p[0] > boxmin[0]
	  && p[1] > boxmin[1]
	  && p[2] > boxmin[2]
	  && p[0] < boxmax[0]
	  && p[1] < boxmax[1]
	  && p[2] < boxmax[2]);
}

