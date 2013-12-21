#ifndef FSM_BIE_SOLVER_INC
#   define FSM_BIE_SOLVER_INC

#include <Eigen/Dense>
#include "BIESpec.h"

class BIESolver
{
    friend struct _ParallelTransferEval;

    public:
        typedef Eigen::Matrix< std::complex<REAL>, Eigen::Dynamic, Eigen::Dynamic>  TMatrixXc;
        typedef Eigen::Matrix< std::complex<REAL>, Eigen::Dynamic, 1>               TVectorXc;

        BIESolver(const BIESpec* spec):
                spec_(spec),
                nEles_( spec->triangles.size() ),
                crho_( spec->speed * spec->density ),
                crhoInv4PI_( spec->speed * spec->density / (4.*M_PI) )
        {
            A_.resize( nEles_, nEles_ );
            B_.resize( nEles_, nEles_ );
            b_.resize( nEles_ );
            x_.resize( nEles_ );
        }

        size_t num_eles() const
        {   return nEles_; }

        const TVectorXc& solution() const
        {   return x_; }

        /* evaluate pressure value using the BI formula */
        std::complex<REAL> eval(const Point3<REAL>& pt) const;

    protected:
        const BIESpec*                      spec_;
        size_t                              nEles_;

        REAL                                crho_;      // speed * rho
        REAL                                crhoInv4PI_;
        REAL                                omega_;     // angular velocity
        REAL                                k_, invK_;  // wave number
        TMatrixXc                           A_, B_;
        TVectorXc                           b_, x_;     // x_ store the solution (i.e., the pressure on elements)
};

#endif
