#include "BIESolver.h"
#include <tbb/parallel_reduce.h>
#include <tbb/blocked_range.h>

using namespace std;

struct _ParallelTransferEval
{
    const BIESolver*        s_;
    const Point3<REAL>&     pt_;

    complex<REAL> ret;

    _ParallelTransferEval(const BIESolver* s, const Point3<REAL>& p):
            s_(s), pt_(p), ret(0,0) {}

    _ParallelTransferEval(_ParallelTransferEval& eval, tbb::split):
            s_(eval.s_), pt_(eval.pt_), ret(0,0) {}

    void join(const _ParallelTransferEval& temp)
    {   ret += temp.ret; }

    void operator() (const tbb::blocked_range<size_t>& range) 
    {
        complex<REAL> localsum(0,0);
        const BIESpec* spec = s_->spec_;
        const complex<REAL> ik = complex<REAL>(0,s_->k_);

        for(size_t ii = range.begin();ii != range.end();++ ii)
        {
            const complex<REAL>& P  = s_->x_(ii);
            const complex<REAL>& dP = spec->normalVel(ii)*ik*s_->crho_;

            // triangle ii
            for(size_t gi = 0;gi < spec->nGaussPts;++ gi)
            {
                // gaussian point --> tri[rId] center
                Vector3<REAL> r  = pt_ - spec->gaussPts[ii][gi];
                const REAL lenr  = r.length();
                const REAL lenr2 = r.length_sqr();                              // r^2
                const complex<REAL> expikr = std::exp(complex<REAL>(0,s_->k_*lenr));

                localsum += expikr*dP * spec->gaussWeights[ii][gi] / (4.*M_PI*lenr);

                const REAL rdotny = r.dot(spec->triNormals[ii]);
                localsum += complex<REAL>(-1,s_->k_*lenr) * expikr * P * rdotny * 
                        spec->gaussWeights[ii][gi] / (4.*M_PI*lenr2*lenr);
            }
        }

        this->ret = localsum;
    }
};

std::complex<REAL> BIESolver::eval(const Point3<REAL>& pt) const
{
    _ParallelTransferEval sum(this, pt);
    //tbb::parallel_reduce(tbb::blocked_range<size_t>(0, nEles_, 300), sum);
    tbb::parallel_deterministic_reduce(tbb::blocked_range<size_t>(0, nEles_, 500), sum);
    //sum(tbb::blocked_range<size_t>(0, nEles_/2));
    //_ParallelTransferEval ss(sum, tbb::split());
    //ss(tbb::blocked_range<size_t>(nEles_/2, nEles_));
    //sum.join(ss);

    return sum.ret;
}

