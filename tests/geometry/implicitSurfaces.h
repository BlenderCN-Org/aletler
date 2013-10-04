#include <cmath>
//#include <functional>



double implicitSphere(double r[3], // pt of interest
		      double c[3], // center
		      double radius) {
  
  double dxsq = pow(r[0] - c[0], 2);
  double dysq = pow(r[1] - c[1], 2);
  double dzsq = pow(r[2] - c[2], 2);
    
  return sqrt(dxsq + dysq + dzsq) - radius;

}


double sphereVolume(double radius) {
  return (4.0/3.0) * M_PI * pow(radius, 3);
}
