#include <iostream>
#include <fstream>
#include <iomanip>
#include <tbb/task_scheduler_init.h>
#include "utils/term_msg.h"
#include "fsm/BIESpec.h"
#include "fsm/BMBIESolver.h"
#include "fsm/dunavant.hpp"
//#include "utils/nano_timer.h"
#include "gauss_quadra/gauss_legendre.h"

using namespace std;

static BIESpec* spec = NULL;

static void load_problem(const char* filename)
{
  spec = load_from_fastbem_input(filename);
  
  const int nGaussQuad = 10;
  REAL gaussQuadX[nGaussQuad], gaussQuadW[nGaussQuad];
  
  gauss_legendre_tbl(nGaussQuad, gaussQuadX, gaussQuadW, 1e-10);
  
  int cc = (nGaussQuad+1)>>1;
  int pp = nGaussQuad>>1;
  if ( nGaussQuad&1 )  // odd
  {
    for(int i = 1;i < cc;++ i)
    {
      gaussQuadX[i+pp] = -gaussQuadX[i];
      gaussQuadW[i+pp] =  gaussQuadW[i];
    }
  }
  else
  {
    for(int i = 0;i < cc;++ i)
    {
      gaussQuadX[i+pp] = -gaussQuadX[i];
      gaussQuadW[i+pp] =  gaussQuadW[i];
    }
  }
  
  int rule = 6;
  int orderNum = dunavant_order_num(rule);
  double* xytab = new double[2*orderNum];
  double*  wtab = new double[orderNum];
  dunavant_rule(rule, orderNum, xytab, wtab);
  vector< Point3<REAL> > xy(orderNum);
  REAL ws = 0;
  for(int i = 0;i < orderNum;++ i)
  {
    xy[i].set(xytab[i*2], xytab[i*2+1], (1.-xytab[i*2]-xytab[i*2+1]));
    cout << xy[i] << endl;
    ws += wtab[i];
  }
  
  spec->init(orderNum, xy.data(), wtab, nGaussQuad, gaussQuadX, gaussQuadW);
}

int main(int argc, char* argv[])
{
  if ( argc != 3 )
  {
    cerr << "Usage: " << argv[0] << " [input.dat] [output-N.dat]" << endl;
  }
  
  tbb::task_scheduler_init init;
  load_problem(argv[1]);
  
  BMBIESolver solver(spec);
  
  PRINT_MSG(" Freq. range [%f %f], %f\n", spec->freqRange.first, spec->freqRange.second, spec->freqDelta);
  ofstream fout(argv[2]);
  if ( fout.fail() ) SHOULD_NEVER_HAPPEN(-1);
  
  fout << setprecision(10);
  // const double st = milli_time_d();
  for(int i = 0;i < spec->nFreq;++ i)
  {
    REAL f0 = spec->freqRange.first + spec->freqDelta*(REAL)i;
    PRINT_MSG(" Freq: %f  size: %d\n", f0, int(solver.num_eles()) );
    solver.solve(f0);
    
    Eigen::VectorXcd wts;
    const Vector3<double> ear(2., 3., 4.);
    solver.computeWeightVector(ear, wts);
    
    /*
    for (size_t i = 0; i < spec->triangles.size(); i++) {
      std::cout << "weight:   " << wts(i) << std::endl;
    }*/

    /*
    const BIESolver::TVectorXc& sol = solver.solution();
    fout << " ~~~~~~~~~~~~~~~~ " << endl;
    fout << " Freq. No. = " << i << ", Freq. = " << f0 << " (Hz)" << endl;
    fout << " Element #          Pressure" << endl;
    
    for(int j = 0;j < solver.num_eles();++ j)
    {
      fout << j+1 << "  ( " << sol(j).real() << ", " << sol(j).imag() << ") " << endl;
    }
    */
    //   double tnow = milli_time_d();
    //   PRINT_MSG("Estimated time remaining: %f hrs\n", (tnow - st) / double(i+1) * double(spec->nFreq-i-1) / 3600.);
    
    for (size_t i = 0; i < spec->nAirTris; i++) {
      fout << wts(i).real() << "     " << wts(i).imag() << std::endl;
    }
  }
  fout.close();
  return 0;
}
