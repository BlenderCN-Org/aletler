//
//  SphericalTools.h
//  soundmath
//
//  Created by Phaedon Sinis on 11/30/12.
//

#ifndef soundmath_SphericalTools_h
#define soundmath_SphericalTools_h

#include <complex>
#include <cmath>

using namespace std;

#include <Eigen/Dense>
using namespace Eigen;


static const complex<double> ii(0,1);


class SphericalTools {
  
public:
    
    // Shows up in formulas as an algebraic way of expressing this conditional...
    // but this should be much faster.
    static int neg_1_power(int p) {
        if (p % 2 == 0) return 1;
        return -1;
    }
    
    static int factorial(int n) {
        if (n == 0) return 1;
        
        int fact = 1;
        for (int i = 1; i <= n; i++) {
            fact *= i;
        }
        
        return fact;
    }
    
    static int double_factorial(int n) {
        
        // initialize
        int fact = 1;
        
        if (n == -1 || n == 0)
            return 1;
        

        // Initialize iteration variable
        int i = 2; // n is even
        if (n % 2 == 1)
            i = 1;  // n is odd
        
        for (; i <= n; i += 2) {
                fact *= i;
            }
        
        return fact;
        
    }
    
    // http://stackoverflow.com/questions/3738384/stable-cotangent
    static double cot(double x) {return tan(M_PI_2 - x); }
    

    
    // Spherical Bessel function of the first kind
    // (inputs to regular spherical basis functions)
    static complex<double> j(int n, complex<double> x) {
        
        if (n < 0) {
            int na = -n;
            return double(neg_1_power(na)) * y(na - 1, x);
        }
        
        if (n == 0) {
            return sin(x) / x;
        } else if (n == 1) {
            return (sin(x) / pow(x, 2)) - (cos(x) / x);
        } else if (n == 2) {
            return (3.0/pow(x, 3) - 1.0/x) * sin(x) - (3.0/pow(x, 2)) * cos(x);
        } else {
            return j(n - 1, x) * (2.0 * (n - 1) + 1) / x - j(n - 2, x);
        }
    }
    
    
    
    // Spherical Bessel function of the second kind
    // (Neumann functions, used for computing j(negative n)
    static complex<double> y(int n, complex<double> x) {

        assert(n >= 0);
        
        if (n == 0) {
            return -cos(x) /  x;
        } else if (n == 1) {
            return (-cos(x) / pow(x, 2)) - (sin(x) / x);
        } else if (n == 2) {
            return (-3.0/pow(x, 2) + 1.0) * (cos(x)/x) - (3.0*sin(x) / pow(x, 2));
        } else {
            // recurrence formula (2.1.86) on pg. 57
            return ((2.0*(n-1) + 1.0) / x) * y(n - 1, x) - y(n - 2, x);
        }
    }
    
    
    
    // Spherical Hankel functions
    // (these are important as inputs to singular spherical basis functions)
    // Gumerov/Duraiswami pg 57
    static complex<double> h(int m, int n, complex<double> x) {
        
        // for now, let's say m == (1):
        assert(m == 1);
        
        complex<double> h_0 = exp(ii*x) / (ii * x);
        
        // Base cases: formula (2.1.84) on pg 57
        if (n == 0) {
            return h_0;
        } else if (n == 1) {
            return -h_0 * (1.0 - (1.0 / (ii * x)));
        } else if (n == 2) {
            return (ii * h_0) * (1.0 - (3.0 / (ii*x)) + (3.0 / x));
        } else {
            
            // recurrence formula (2.1.86) on pg. 57
            return ((2.0*(n-1) + 1) / x) * h(m, n - 1, x) - h(m, n - 2, x);
        }
        
    }
    
    
    // Associated Legendre functions of order m, degree n
    static complex<double> P(int m, int n, double mu) {
        
        // restrictions on m & n
        assert(n >= 0);
        //if (n < 0) return 0;
        assert(m <= n);
        //if (m > n) return 0;
        
        // base case
        if (n == 0) return 1;
        
        // Special cases (Gumerov & Duraiswami, pg 47)
        if (m == 1 && n == 1)
            return -sqrt(1 - pow(mu, 2));
        else if (m == 1 && n == 2)
            return -3 * mu * sqrt(1 - pow(mu, 2));
        else if (m == 2 && n == 2)
            return 3 * (1 - pow(mu, 2));
        
        // Recurrence relations (Volker Schonefeld, pg 5)
        if (m == n) {
            return neg_1_power(n) * double_factorial(2*n - 1) * pow(1 - pow(mu, 2), n*0.5);
            
        } else if (n == m + 1) {
            return mu * double(2*m + 1) * P(m, m, mu);

        } else {
            complex<double> left_recursion = P(m, n - 1, mu);
            complex<double> right_recursion = P(m, n - 2, mu);
            
            return (mu * double(2*n - 1) * left_recursion - double(n + m - 1) * right_recursion) / double(n - m);
        }
    }
    
    
    // Spherical harmonics (Gumerov & Duraiswami, pg 49)
    static complex<double> Y(int m, int n, double theta, double phi) {
        
        // 
        assert(n >= 0);
        assert(m >= -n && m <= n);
        
        double root = sqrt( (2*n + 1) * factorial(n - abs(m)) / (4 * M_PI * factorial(n + abs(m)) ));

        return neg_1_power(m) * root * P(abs(m), n, cos(theta)) * exp(ii * double(m) * phi);
    }
    
    
    // Spherical basis functions
    static complex<double> R(int m, int n, complex<double> k, Vector3d rvec) {
        
        double r = rvec.norm();
        double theta = acos(rvec[2] / r);
        double phi = cot(rvec[1]/rvec[0]);
        
        return j(n, k * r) * Y(m, n, theta, phi);
    }
    
    static complex<double> S(int m, int n, complex<double> k, Vector3d rvec) {
        
        double r = rvec.norm();
        double theta = acos(rvec[2] / r);
        double phi = cot(rvec[1]/rvec[0]);
        
        return h(1, n, k * r) * Y(m, n, theta, phi);
    }
    
    
    static complex<double> Greens(complex<double> k, Vector3d rvec) {
        return M_1_PI * 0.25 * exp(ii * k * rvec.norm()) / rvec.norm();
    }
    
private:
    
    
};

#endif
