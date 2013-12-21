#include "CBIESolver.h"
#include <tbb/parallel_for.h>
#include "utils/term_msg.h"

using namespace std;

CBIESolver::CBIESolver(const BIESpec* spec):
        BIESolver(spec), cached_(false),
        derCacheD_(boost::extents[ spec->triangles.size() ][ spec->triangles.size() ][ spec->nGaussPts ]),
        derCacheS_(boost::extents[ spec->triangles.size() ][ spec->triangles.size() ][ spec->nGaussPts ]),
        qr_(nEles_, nEles_)
{ }

/*
 * Construct the linear system in parallel
 */
struct _ParallelCBIEConstruct
{
    CBIESolver*     s_;
    const BIESpec*  spec_;

    _ParallelCBIEConstruct(CBIESolver* s):s_(s), spec_(s->spec_) { }

    void operator() (const tbb::blocked_range<size_t>& r) const
    {
        for(size_t ii = r.begin();ii != r.end();++ ii) construct_row(ii);
    }

    void construct_row(size_t rowId) const
    {
        s_->A_(rowId,rowId) = complex<REAL>(0.5,0);
        s_->B_(rowId,rowId) = complex<REAL>(-0.5,0)*s_->crho_; // i/(2k) * ik * rho*c

        for(size_t cId = 0;cId < s_->nEles_;++ cId)
        {
            if ( cId == rowId ) continue;
            //// triangle[rowId] <----- triangle[cId]
            //// construct S matrix:   S q(y) = \int G q dS(y)
            //// construct D matrix
            construct_SD(rowId, cId);
        }
        construct_R(rowId);
    }

    void construct_R(size_t rId) const
    {
        complex<REAL> sum1(0,0);     // int e^{ikR} dt
        const HyperSingularIntRec& rec = spec_->triHyperSing[rId];

        for(int ii = 0;ii < 6;++ ii)
        for(size_t jj = 0;jj < rec.n;++ jj)
        {
            sum1 += std::exp(complex<REAL>(0, s_->k_*rec.R[ii][jj]))*rec.W[ii][jj];
        }

        const complex<REAL> i4pik = complex<REAL>(0,1./(4.*M_PI*s_->k_)); // *= i/(4*pi*k)
        s_->B_(rId,rId) -= sum1 * i4pik * complex<REAL>(0, s_->k_)* s_->crho_;  // *ik*rho*c
    }

    void construct_SD(size_t rId, size_t cId) const
    {
        complex<REAL> sij(0,0);
        complex<REAL> dij(0,0);

        for(size_t gi = 0;gi < spec_->nGaussPts;++ gi)
        {
            // gaussian point --> tri[rId] center
            Vector3<REAL> r = spec_->triCenters[rId] - spec_->gaussPts[cId][gi];
            REAL lenr  = r.length();
            REAL lenr2 = lenr*lenr;                                 // r^2

            s_->derCacheS_[rId][cId][gi] = spec_->gaussWeights[cId][gi]*
                    s_->crho_ / (4.*M_PI*lenr);
            /* ik*c*rho*exp(ikr)/(4*PI*r)*/
            sij += complex<REAL>(0, s_->k_)*std::exp(complex<REAL>(0,s_->k_*lenr))*
                    s_->derCacheS_[rId][cId][gi];

            s_->derCacheD_[rId][cId][gi] = -spec_->gaussWeights[cId][gi] * 
                    r.dot(spec_->triNormals[cId]) / (4.*M_PI*lenr2*lenr);
            /* (-1+ikr)*exp(ikr)*-<r.n>/(4*PI*r^3) */
            dij += complex<REAL>(-1,s_->k_*lenr)*std::exp(complex<REAL>(0,s_->k_*lenr))*
                    s_->derCacheD_[rId][cId][gi];
        }
        s_->B_(rId,cId) = sij;
        s_->A_(rId,cId) = dij;
    }
};

void CBIESolver::solve(REAL freq)
{
    omega_ = freq * 2. * M_PI;
    k_     = omega_ / spec_->speed;

    PRINT_MSG("Construct the linear system ...\n");
    tbb::parallel_for(tbb::blocked_range<size_t>(0, nEles_), 
                      _ParallelCBIEConstruct(this));
    b_ = B_ * spec_->normalVel;

    PRINT_MSG("QR Factorization of A ...\n");
    qr_.compute(A_); 

    PRINT_MSG("Solve the linear system ...\n");
    x_ = qr_.solve(b_);
}

