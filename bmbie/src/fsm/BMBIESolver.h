#ifndef BM_BIE_SOLVER_INC
#   define BM_BIE_SOLVER_INC

#include <boost/multi_array.hpp>
#include "BIESolver.h"

/*
 * Helmholtz BIE solver using Burton-Miller formula 
 */
class BMBIESolver : public BIESolver
{
    friend struct _ParallelBMBIEConstruct;

    public:
        BMBIESolver(const BIESpec* spec):BIESolver(spec), qr_(nEles_, nEles_)
        { }

        void solve(REAL freq);

    private:
        Eigen::ColPivHouseholderQR<TMatrixXc> qr_;
        //Eigen::FullPivLU<TMatrixXc> qr_;
};

#endif
