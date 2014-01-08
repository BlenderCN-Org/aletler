#include "BIESpec.h"
#include <sstream>
#include <fstream>
#include <tbb/parallel_for.h>
#include "geometry/Triangle.hpp"
#include "utils/term_msg.h"
#include "utils/strings.hpp"

using namespace std;


void HyperSingularIntRec::update(const Point3<REAL>& p0, 
        const Point3<REAL>& p1, const Point3<REAL>& p2)
{
    Point3<REAL> ctr = (p0 + p1 + p2)/(REAL)3;  // centroid
    Vector3<REAL> v0 = ctr-p0; 
    Vector3<REAL> v1 = ctr-p1; 
    Vector3<REAL> v2 = ctr-p2; 
    const REAL l0 = v0.length();
    const REAL l1 = v1.length();
    const REAL l2 = v2.length();

    REAL d;
    Vector3<REAL> v10 = p1 - p0;
    v10.normalize();
    d = v10.dot(v0);
    this->h[0] = sqrt(v0.length_sqr() - d*d);
    this->theta[0] = acos(this->h[0] / l0);
    this->theta[1] = acos(this->h[0] / l1);
    assert(this->theta[0]<M_PI_2 && this->theta[1]<M_PI_2);

    Vector3<REAL> v21 = p2 - p1;
    v21.normalize();
    d = v21.dot(v1);
    this->h[1] = sqrt(v1.length_sqr() - d*d);
    this->theta[2] = acos(this->h[1] / l1);
    this->theta[3] = acos(this->h[1] / l2);
    assert(this->theta[2]<M_PI_2 && this->theta[3]<M_PI_2);

    Vector3<REAL> v02 = p0 - p2;
    v02.normalize();
    d = v02.dot(v2);
    this->h[2] = sqrt(v2.length_sqr() - d*d);
    this->theta[4] = acos(this->h[2] / l2);
    this->theta[5] = acos(this->h[2] / l0);
    assert(this->theta[4]<M_PI_2 && this->theta[5]<M_PI_2);
}

void HyperSingularIntRec::update_gaussian_quadra_samples(
        size_t n, const REAL* x, const REAL* w)    // $TESTED$
{
    this->n = n;
    for(int ii = 0;ii < 6;++ ii)
    {
        this->R[ii].resize(n);
        this->W[ii].resize(n);

        const REAL vv = this->theta[ii] * 0.5;
        for(size_t j = 0;j < n;++ j)
        {
            this->W[ii][j] = w[j] * vv;
            this->R[ii][j] = this->h[ii/2] / cos(vv * x[j] + vv);
        }
    }
}

// ----------------------------------------------------------------------------

struct _ParallelBIESpecInit
{
    BIESpec*            spec;
    const Point3<REAL>* gaussPts;
    const REAL*         gaussPtWeights;
    size_t              nGaussQuadra;
    const REAL*         gaussQuadX;
    const REAL*         gaussQuadW;

    _ParallelBIESpecInit(BIESpec* s, const Point3<REAL>* gPts, 
            const REAL* gW, size_t nGQ, const REAL* gqX, 
            const REAL* gqW):
            spec(s), gaussPts(gPts), 
            gaussPtWeights(gW), nGaussQuadra(nGQ), 
            gaussQuadX(gqX), gaussQuadW(gqW)
    { }

    void operator() (const tbb::blocked_range<size_t>& r) const
    {
        for(size_t ii = r.begin();ii != r.end();++ ii) init_tri(ii);
    }

    void init_tri(size_t tId) const
    {
        const Point3<REAL>& p0 = spec->vertices[ spec->triangles[tId].x ];
        const Point3<REAL>& p1 = spec->vertices[ spec->triangles[tId].y ];
        const Point3<REAL>& p2 = spec->vertices[ spec->triangles[tId].z ];

        spec->triCenters[tId] = (p0+p1+p2)/3;
        spec->triNormals[tId] = Triangle<REAL>::normal(p0, p1, p2);
        spec->triAreas[tId]   = spec->triNormals[tId].normalize2() * 0.5;

        // gaussian quadrature points on the triangle
        for(size_t jj = 0;jj < spec->nGaussPts;++ jj)
        {
            spec->gaussWeights[tId][jj] = gaussPtWeights[jj]*spec->triAreas[tId];
            spec->gaussPts[tId][jj] = p0*gaussPts[jj].x + p1*gaussPts[jj].y + p2*gaussPts[jj].z;
        }
        // for hyper singular computation   $TESTED$
        spec->triHyperSing[tId].update(p0, p1, p2);
        spec->triHyperSing[tId].update_gaussian_quadra_samples(nGaussQuadra, gaussQuadX, gaussQuadW);
    }
};

void BIESpec::init(size_t nGauss, const Point3<REAL>* gPts, 
        const REAL* gW, size_t nGQ, const REAL* gqX, const REAL* gqW)
{
    this->nGaussPts = nGauss;
    this->gaussWeights.resize( boost::extents[this->triangles.size()][nGauss] );
    this->gaussPts.resize( boost::extents[this->triangles.size()][nGauss] );

    //_ParallelBIESpecInit pp(this, gPts, gW, nGQ, gqX, gqW);
    //pp(tbb::blocked_range<size_t>(0, this->triangles.size()));
    tbb::parallel_for(tbb::blocked_range<size_t>(0, this->triangles.size()),
                      _ParallelBIESpecInit(this, gPts, gW, nGQ, gqX, gqW));
}

// ----------------------------------------------------------------------------

BIESpec* load_from_fastbem_input(const char* filename)
{
    BIESpec* ret = new BIESpec;

  ret->nAirTris = 0;
  
    char text[1024];
    int ntgl, nvtx, nfp;

    // ==================================================================
    //// NOTE: assume no empty line
    std::ifstream fin(filename);
    if ( fin.fail() )
    {
        PRINT_ERROR("Cannot load file: %s\n", filename);
        exit(1);
    }

    fin.getline(text, 1024);            // first line
    fin.getline(text, 1024);            // 2nd line "Complete 1/3"
    fin.getline(text, 1024);            // 3rd line "Full 0 0.d0"
    fin.getline(text, 1024);            // 4th line "# B.E. # Nodes # F.P. # F.C."
    std::istringstream(text) >> ntgl >> nvtx >> nfp;

    fin.getline(text, 1024);            // 5th line
    int ia, ib;
    std::istringstream(text) >> ia >> ib;
    if ( ia || ib )
    {
        PRINT_ERROR("Unsupported problem setting at L. 5\n");
        exit(1);
    }
    fin.getline(text, 1024);            // 6th line
    std::istringstream(text) >> ia;
    if ( ia )
    {
        PRINT_ERROR("Unsupported problem setting at L. 6\n");
        exit(1);
    }

    fin.getline(text, 1024);            // 7th line: speed and density
    std::istringstream(text) >> ret->speed >> ret->density;

    fin.getline(text, 1024);                // L.8
    std::istringstream(text) >> ret->freqRange.first >> ret->freqRange.second >> ia;
    ret->freqDelta = ia == 1 ? 0 : (ret->freqRange.second - ret->freqRange.first) / (REAL)(ia-1);
    ret->nFreq = ia;

    fin.getline(text, 1024);                // L.9
    fin.getline(text, 1024);                // L.10
    if ( text[0] != '$' || text[2] != 'N' )  // Start to read number of nodes
    {
        PRINT_ERROR("incorrect file format at L. 7\n");
        exit(1);
    }

    //// load vertices
    int id;
    REAL xx, yy, zz;
    ret->vertices.resize(nvtx);
    for(int i = 0;i < nvtx;++ i)
    {
        fin.getline(text, 1024);
        std::istringstream(text) >> id >> xx >> yy >> zz;
        ret->vertices[id-1].set(xx, yy, zz);
    }

    //// load boundary conditions
    fin.getline(text, 1024);
    if ( text[0] != '$' || text[2] != 'E' )
    {
        PRINT_ERROR("incorrect file format for boundary condition\n");
        exit(1);
    }
    ret->triangles.resize(ntgl);        // S
    ret->triCenters.resize(ntgl);       //
    ret->triNormals.resize(ntgl);       //
    ret->triAreas.resize(ntgl);         //
    ret->normalVel.resize(ntgl);        // S
    ret->triHyperSing.resize(ntgl);

    int idx, idy, idz;
    char c1, c2;
    REAL v1, v2;
    for(int i = 0;i < ntgl;++ i)
    {
        fin.getline(text, 1024);
        std::istringstream(text) >> id >> idx >> idy >> idz >> ia >> c1 >> v1 >> c2 >> v2;
        if (ia != 2)
        {
            PRINT_ERROR("Only Neumann boundary condition is supported now\n");
            exit(1);
        }
      if (v1 != 0.0) {
        ret->nAirTris++;
      }
        ret->triangles[id-1].set(idx-1, idy-1, idz-1);
        ret->normalVel(id-1) = complex<REAL>(v1, v2);
    }
    if ( fin.fail() ) 
    {
        PRINT_ERROR("unexpected ending of file\n");
        exit(1);
    }
    fin.close();

    return ret;
}

/*
const size_t MAX_SER_N = 9;

REAL _sec_int_series(size_t n, REAL h, REAL theta, REAL* out) // $TESTED$
{
    REAL* tanArr = new REAL[n];
    REAL* secArr = new REAL[n];

    const REAL sinv = sin(theta);
    tanArr[1] = tan(theta);
    secArr[1] = 1. / cos(theta);
    for(size_t i = 2;i < n;++ i)
    {
        tanArr[i] = tanArr[i-1]*tanArr[1];
        secArr[i] = secArr[i-1]*secArr[1];
    }

    assert(n >= 2);
    out[0] = theta; // m = 0
    out[1] = log(secArr[1] + tanArr[1]);

    for(size_t i = 2;i < n;i += 2)
    {
        size_t m = i/2;
        out[i] = 0;
        for(size_t l = 0, t = 1;l < m;++ l, t += 2) // t = 2*l+1
            out[i] += tanArr[t]*BIN_COEFF[m-1][l]/(REAL)(t);
        if ( i+1 < n )
            out[i+1] = secArr[i]*sinv/(REAL)i + out[i-1]*(REAL)(i-1)/(REAL)i;
    }

    delete []tanArr;
    delete []secArr;
}
*/
