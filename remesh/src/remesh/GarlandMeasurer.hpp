#ifndef GARLAND_MEASURER_INC
#   define GARLAND_MEASURER_INC

#include "config.h"
#include "sc/Matrix3.hpp"
#include "geometry/Point3.hpp"
#include "geometry/Triangle.hpp"
#include <boost/unordered_map.hpp>

template <class THEMesh>
class GarlandMeasurer
{
    public:
        typedef typename THEMesh::TEdge     TEdge;
        typedef typename THEMesh::THalfEdge THalfEdge;
        typedef typename THEMesh::TVertex   TVertex;

        void init(THEMesh* mesh);

        REAL collapse_cost(TEdge* e, Point3<REAL>& v, Vector3<REAL>& u);

        void edge_collapsed(TVertex* v0, TVertex* v1, TVertex* vnew)
        {
            assert(vQuad_.count(v0) && vQuad_.count(v1));
            vQuad_[vnew] = vQuad_[v0] + vQuad_[v1];
            vQuad_.erase(v0);
            vQuad_.erase(v1);
        }

    private:
        void update_quadric(TVertex* v);

    private:
        struct _VtxQuadric
        {
            Matrix3<REAL>   Q;
            Vector3<REAL>   b;
            REAL            d;

            _VtxQuadric() { }
            _VtxQuadric(const _VtxQuadric& v):Q(v.Q), b(v.b), d(v.d) { }

            _VtxQuadric& operator = (const _VtxQuadric& rhs)
            {
                Q = rhs.Q;
                b = rhs.b;
                d = rhs.d;
                return *this;
            }

            _VtxQuadric& operator += (const _VtxQuadric& rhs)
            {
                Q += rhs.Q;
                b += rhs.b;
                d += rhs.d;
                return *this;
            }

            _VtxQuadric operator + (const _VtxQuadric& rhs) const
            {
                _VtxQuadric ret;
                ret.Q = Q + rhs.Q;
                ret.b = b + rhs.b;
                ret.d = d + rhs.d;
                return ret;
            }
        };

        THEMesh*    mesh_;

        boost::unordered_map< TVertex*, _VtxQuadric > vQuad_;
};

// -------------------------------------------------------------

template <class THEMesh>
void GarlandMeasurer<THEMesh>::init(THEMesh* mesh)
{
    assert(mesh);
    mesh_ = mesh;

    vQuad_.clear();

    int nv;
    TVertex* const* vs = mesh->vertices(nv);
    for(int i = 0;i < nv;++ i) update_quadric(vs[i]);
}

template <class THEMesh>
void GarlandMeasurer<THEMesh>::update_quadric(TVertex* v)
{
    vQuad_[v] = _VtxQuadric();

    _VtxQuadric& qout = vQuad_[v];
    _VtxQuadric vrec;

    THalfEdge* he0 = v->he_;
    THalfEdge* lastHe = v->he_;
    THalfEdge* h   = he0->flip_->next_;
    do
    {
        Vector3<REAL> nml = Triangle<REAL>::normal(
                lastHe->headVtx_->pos_,
                v->pos_, h->headVtx_->pos_);
        REAL a = nml.normalize2();

        const REAL   d = nml.dot(v->pos_);
        vrec.Q.cols[0] = nml * nml.x;
        vrec.Q.cols[1] = nml * nml.y;
        vrec.Q.cols[2] = nml * nml.z;
        vrec.Q        *= a;
        vrec.b         = nml*(d*a);
        vrec.d         = d*d*a;

        qout += vrec;

        lastHe = h;
        h = h->flip_->next_;
    } while ( lastHe != he0 );
}

template <class THEMesh>
REAL GarlandMeasurer<THEMesh>::collapse_cost(
        TEdge* e, Point3<REAL>& v, Vector3<REAL>& u)
{
    assert( vQuad_.count(e->he_->headVtx_) && 
            vQuad_.count(e->he_->tailVtx_) );

    _VtxQuadric m = vQuad_[e->he_->headVtx_] + 
                    vQuad_[e->he_->tailVtx_];
    Matrix3<REAL> invQ;
    if ( m.Q.inverse(invQ) >= 0 )
    {
        Vector3<REAL> vv = invQ*m.b;
        v.set(vv.x, vv.y, vv.z);
        u.zero();
        return vv.dot(m.Q*vv) - 2.*m.b.dot(vv) + m.d;
    }

    // use head vertex
    Vector3<REAL> vv = e->he_->headVtx_->pos_;
    REAL e0 = vv.dot(m.Q*vv) - 2.*m.b.dot(vv);

    // use tail vertex
    vv = e->he_->tailVtx_->pos_;
    REAL e1 = vv.dot(m.Q*vv) - 2.*m.b.dot(vv);

    if ( e0 > e1 ) 
    {
        e0 = e1;
        v = e->he_->tailVtx_->pos_;
    }
    else
    {
        v = e->he_->headVtx_->pos_;
    }

    vv = (e->he_->headVtx_->pos_ + e->he_->tailVtx_->pos_)*0.5;
    e1 = vv.dot(m.Q*vv) - 2.*m.b.dot(vv);
    if ( e1 < e0 )  
    {
        v = vv;
        return e1 + m.d;
    }
    return e0 + m.d;
}

#endif
/*
        void remove_vertex(TVertex* v)
        {   vQuad_.erase(v); }

        void add_vertex(TVertex* v)
        {
            assert(v);
            update_quadric(v);
        }
*/
