#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include "gauss_legendre.h"

using namespace std;

#ifndef PI
	#define PI 3.1415926535897932384626433832795028841971693993751
#endif
#ifndef FABS
	#define FABS(a) ((a)>=0?(a):-(a))
#endif


double f(double x, void* data)
{
	return sin(x);
}

int main(int argc, char* argv[])
{

	/* numerical approximation of integral */
	double approx;		

	/* true value of int(sin(x), x=0..Pi) = 2.0*/
	double exact = 2.0; 

	/* approximation error */
	double error;       

	int i;

	printf("Numerical Approximation of int(sin(x), x=0..Pi) by Gauss-Legendre Quadrature:\n");
	for (i=2;i<=128;i++)
	{
		approx = gauss_legendre(i,f,NULL,0,PI);
		error = approx-exact;
		printf("n = %4d: error = %.15g\n",i,FABS(error));
	}

    int n = 5;
    double x[(5+1)>>1];
    double w[(5+1)>>1];
    gauss_legendre_tbl(n, x, w, 1e-10);
    for(int i = 0;i < (n+1)>>1;++ i)
        cout << x[i] << ' ' << w[i] << endl;
}

