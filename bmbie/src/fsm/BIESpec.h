#ifndef BIE_SPEC_INC
#   define BIE_SPEC_INC

#include <stdint.h>
#include <vector>
#include <valarray>
#include <complex>
#include <Eigen/Dense>
#include <boost/multi_array.hpp>
#include "sc/Vector3.hpp"
#include "geometry/Point3.hpp"
#include "config.h"


struct HyperSingularIntRec
{
    REAL                h[3];
    REAL                theta[6];
    size_t              n;          // number of gaussian quadrature points
    std::vector<REAL>   R[6];       // R value sampled at gaussian quadrature abscissa
    std::vector<REAL>   W[6];

    void update(const Point3<REAL>& p0,
                const Point3<REAL>& p1,
                const Point3<REAL>& P2);

    void update_gaussian_quadra_samples(size_t n, const REAL* x, const REAL* w);
};

/*
 * specification of a BIE configuration
 */
struct BIESpec
{
    typedef Eigen::Matrix< std::complex<REAL>, Eigen::Dynamic, 1>               TVectorXc;

    REAL                            speed;          // sound speed
    REAL                            density;        // media density

    /* -------------------------------------------- 
     * Boundary specification
     * -------------------------------------------- */
    std::vector< Point3<REAL> >         vertices;
    std::vector< Tuple3ui >             triangles;      // indices of triangle vertices
    std::vector< Point3<REAL> >         triCenters;     // center point of each triangle
    std::vector< Vector3<REAL> >        triNormals;     // normals at each triangle
    std::valarray<REAL>                 triAreas;       // area of each triangles
    TVectorXc                           normalVel;      // normal velocity at each element
    std::vector< HyperSingularIntRec >  triHyperSing;

    size_t                                  nGaussPts;
    boost::multi_array< REAL, 2 >           gaussWeights;
    boost::multi_array< Point3<REAL>, 2 >   gaussPts;

    std::pair<REAL, REAL>               freqRange;
    int                                 nFreq;
    REAL                                freqDelta;
  
  // # of trianges on the air/free surface boundary. Assumes these appear first in the file
  size_t nAirTris;

    void init(size_t nGauss, const Point3<REAL>* gPts, const REAL* gW,
              size_t nGQ, const REAL* gqX, const REAL* gqW);
};

BIESpec* load_from_fastbem_input(const char* filename);

#endif

#if 0
const uint32_t BIN_COEFF[][10] = {
    {1,0,0,0,0,0,0,0,0,0},
    {1,1,0,0,0,0,0,0,0,0},
    {1,2,1,0,0,0,0,0,0,0},
    {1,3,3,1,0,0,0,0,0,0},
    {1,4,6,4,1,0,0,0,0,0},
    {1,5,10,10,5,1,0,0,0,0},
    {1,6,15,20,15,6,1,0,0,0},
    {1,7,21,35,35,21,7,1,0,0},
    {1,8,28,56,70,56,28,8,1,0},
    {1,9,36,84,126,126,84,36,9,1}};

const uint32_t FACTORIAL_VAL[] = {
    1, 1, 2, 6, 24, 120, 720, 5040, 
    40320, 362880, 3628800, 39916800};

// -------------------------------------------------------------------
#endif
