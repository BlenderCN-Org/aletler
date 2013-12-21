#include "BMBIESolver.h"
#include <tbb/parallel_for.h>
#include "utils/term_msg.h"

using namespace std;

#if 0
#warning ----------------- REMOVE ME ------------------
complex<double> deriv_analy22(
        const double K,
        const Point3d& p0, const Point3d& p1, 
        const Vector3d& n0, const Vector3d& n1)
{
    Vector3d r = p0 - p1;
    double  lr = r.length();
    complex<double> v1(-3/(lr*lr)+K*K, 3.*K/lr);
    v1 *= (r.dot(n1)*r.dot(n0));
    complex<double> v2(1, -K*lr);   // 1 - ikr
    v2 *= (n1.dot(n0));
    return (v1+v2)*std::exp(complex<double>(0, K*lr)) / (4*M_PI*lr*lr*lr);
}
complex<double> deriv_analy00(const double K, const Point3d& p0, const Point3d& p1, 
                              const Vector3d& n0, const Vector3d& n1)
{
    Vector3d r = p0 - p1;
    double  lr = r.length();
    return -r.dot(n1)*std::exp(complex<double>(0, K*lr)) * complex<double>(-1, K*lr) /(4*M_PI*lr*lr*lr);
}
complex<double> deriv_analy01(const double K, const Point3d& p0, const Point3d& p1, 
                              const Vector3d& n0, const Vector3d& n1)
{
    Vector3d r = p0 - p1;
    double  lr = r.length();
    return r.dot(n0)*std::exp(complex<double>(0, K*lr)) * complex<double>(-1, K*lr) /(4*M_PI*lr*lr*lr);
}
#endif

/*
 * Construct the linear system in parallel
 */
struct _ParallelBMBIEConstruct
{
    BMBIESolver*     s_;
    const BIESpec*  spec_;

    _ParallelBMBIEConstruct(BMBIESolver* s):s_(s), spec_(s->spec_) { }

    void operator() (const tbb::blocked_range<size_t>& r) const
    {
        for(size_t ii = r.begin();ii != r.end();++ ii) construct_row(ii);
    }

    void construct_row(size_t rowId) const
    {
        /*
         * the 1/2 will be canceled by +beta* ik/2 if we choose beta = i/k
         * s_->A_(rowId,rowId) = complex<REAL>(0.5,0);
         */
        s_->A_(rowId,rowId) = complex<REAL>(0,0);
        /*
         * The i/(2K) will be canceled by the -beta/2 if we choose beta = i/k
        */
        s_->B_(rowId,rowId) = complex<REAL>(0,0);

        for(size_t cId = 0;cId < s_->nEles_;++ cId)
        {
            if ( cId == rowId ) continue;
            //// triangle[rowId] <----- triangle[cId]
            //// construct S matrix:   S q(y) = \int G q dS(y)
            //// construct D matrix
            construct_SD(rowId, cId);   // only affect the off-diagonal terms
        }
        construct_R(rowId);
    }

    void construct_R(size_t rId) const
    {
        const BIESpec*  spec = spec_;

        complex<REAL> sum1(0,0);     // int e^{ikR} dt
        complex<REAL> sum2(0,0);     // int e^{ikR}/(4pi*R) dt
        const HyperSingularIntRec& rec = spec->triHyperSing[rId];

        for(int ii = 0;ii < 6;++ ii)
        for(size_t jj = 0;jj < rec.n;++ jj)
        {
            const complex<REAL> expikrW = std::exp(complex<REAL>(0, s_->k_*rec.R[ii][jj]))*rec.W[ii][jj];
            sum1 += expikrW;
            sum2 += (expikrW / rec.R[ii][jj]);
        }

        const complex<REAL> i4pik = complex<REAL>(0,1./(4.*M_PI*s_->k_)); // *= i/(4*pi*k)
        s_->B_(rId,rId) -= sum1 * i4pik * complex<REAL>(0, s_->k_)* s_->crho_;  // *ik*rho*c
        s_->A_(rId,rId) -= sum2 * i4pik;
    }

    void construct_SD(size_t rId, size_t cId) const
    {
        const BIESpec*  spec = spec_;

        complex<REAL> rightij(0,0);
        complex<REAL> leftij(0,0);
        const complex<REAL> ik = complex<REAL>(0,s_->k_);

        for(size_t gi = 0;gi < spec->nGaussPts;++ gi)
        {
            // gaussian point --> tri[rId] center
            Vector3<REAL> r = spec->triCenters[rId] - spec->gaussPts[cId][gi];
            const REAL lenr  = r.length();
            const REAL lenr2 = r.length_sqr();                              // r^2
            const complex<REAL> expikr = std::exp(complex<REAL>(0,s_->k_*lenr));

            const complex<REAL> cacheS = spec->gaussWeights[cId][gi]*
                    s_->crho_ / (4.*M_PI*lenr);                 // independent of k
            /* ik*c*rho*exp(ikr)/(4*PI*r) */
            rightij += ik*expikr*cacheS;  //std::exp(complex<REAL>(0,s_->k_*lenr))*

            const REAL invss = 1. / (4.*M_PI*lenr2*lenr);
            const REAL rdotny = r.dot(spec->triNormals[cId]);
            const REAL rdotnx = r.dot(spec->triNormals[rId]);
            const complex<REAL> cacheD = -spec->gaussWeights[cId][gi] * rdotny * invss;
            const complex<REAL> cacheM =  spec->gaussWeights[cId][gi] * rdotnx * invss * s_->crho_;
            /* (-1+ikr)*exp(ikr)*-<r.n>/(4*PI*r^3) */
            leftij  += complex<REAL>(-1,s_->k_*lenr) * expikr * cacheD;
            /* (-r-i/k)*exp(ikr)*<r.n>/(4*PI*r^3)*rho*c*ik */
            rightij += ik*complex<REAL>(-lenr,-1./s_->k_) * expikr * cacheM;

            /* ---- H matrix part ---- */
            complex<REAL> v1(-3./lenr, s_->k_ - 3./(lenr2*s_->k_));   // -3/r + (k-3/(r*r*k))i
            v1 *= (rdotny*rdotnx);
            complex<REAL> v2(lenr, 1./s_->k_);                        // r - i/k
            v2 *= spec->triNormals[cId].dot(spec->triNormals[rId]);
            leftij += (v1+v2)*expikr * (invss * spec->gaussWeights[cId][gi]);
        }

        s_->B_(rId,cId) = rightij;
        s_->A_(rId,cId) = leftij;
    }
};

void BMBIESolver::solve(REAL freq)
{
    omega_ = freq * 2. * M_PI;
    k_     = omega_ / spec_->speed;

    PRINT_MSG("Construct the linear system ...\n");
    tbb::parallel_for(tbb::blocked_range<size_t>(0, nEles_), 
                      _ParallelBMBIEConstruct(this));
    b_ = B_ * spec_->normalVel;

    PRINT_MSG("QR Factorization of A ...\n");
    qr_.compute(A_); 

    PRINT_MSG("Solve the linear system ...\n");
    x_ = qr_.solve(b_);
}

