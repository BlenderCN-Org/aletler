#ifndef CBIE_SOLVER_INC
#   define CBIE_SOLVER_INC

#include <boost/multi_array.hpp>
#include "BIESolver.h"

/*
 * Helmholtz CBIE Solver 
 */
class CBIESolver : public BIESolver
{
    friend struct _ParallelCBIEConstruct;

    public:
        typedef boost::multi_array<REAL, 3>     TDCArray;

        CBIESolver(const BIESpec* spec);

        void solve(REAL freq);

    private:
        bool                cached_;        // used for sweeping
        TDCArray            derCacheD_;
        TDCArray            derCacheS_;
        Eigen::ColPivHouseholderQR<TMatrixXc> qr_;
};

#endif
