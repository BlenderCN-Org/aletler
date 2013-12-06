#ifndef HOPPE_MEASURER_INC
#   define HOPPE_MEASURER_INC

//#include <Eigen/QR>
//#include <Eigen/Dense>
#include <boost/unordered_map.hpp>
#include "config.h"
#include "geometry/Triangle.hpp"

template <class THEMesh>
class HoppeMeasuerer
{
    public:
        typedef typename THEMesh::THalfEdge THalfEdge;
        typedef typename THEMesh::TEdge     TEdge;
        typedef typename THEMesh::TVertex   TVertex;
        typedef typename THEMesh::TFace     TFace;

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
        /*
         * the attribute at each face is linearized by
         *         g[i]^T p + d[i]
         * where i=1,2,3 indicates the x,y,z component of the modal shape
         */
        struct _FaceAttriVec
        {
            Eigen::Matrix<REAL, 4, 1>   av[3];
            Vector3<REAL>               nml;
            REAL                        area;
        };

        /*
         * compute the quadric for all the vertices
         */
        void update_vertex_quadric(boost::unordered_map< TFace*, _FaceAttriVec >& fAttriVec);

        void update_face_attri_vec(boost::unordered_map< TFace*, _FaceAttriVec >& fAttriVec);

    private:
        typedef Eigen::Matrix<REAL, 6, 6>   TQuadMat;
        typedef Eigen::Matrix<REAL, 6, 1>   TQuadVec;

        struct _VtxQuadric
        {
            TQuadMat    Q;
            TQuadVec    b;
            REAL        d;

            _VtxQuadric():d(0)
            { 
                Q.setZero(); 
                b.setZero();
            }

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

        Eigen::Matrix<REAL, 7, 7>                     A_;
        Eigen::Matrix<REAL, 7, 1>                     b_;
        Eigen::Matrix<REAL, 7, 1>                     x_;
};

// -------------------------------------------------------------

template <class THEMesh>
void HoppeMeasuerer<THEMesh>::init(THEMesh* mesh)
{
    assert(mesh);
    mesh_ = mesh;

    vQuad_.clear();
    A_.setZero();

    boost::unordered_map< TFace*, _FaceAttriVec > fAttriVec;
    update_face_attri_vec(fAttriVec);
    update_vertex_quadric(fAttriVec);
}

template <class THEMesh>
void HoppeMeasuerer<THEMesh>::update_face_attri_vec(
        boost::unordered_map< TFace*, _FaceAttriVec >& fAttriVec)
{
    Eigen::Matrix<REAL, 4, 4> A;
    Eigen::Matrix<REAL, 4, 1> b;
    A(3,3) = 0; b(3) = 0;
    A(0,3) = 1; A(1,3) = 1; A(2,3) = 1;

    int nf;
    TFace* const* fs = mesh_->faces(nf);
    for(int i = 0;i < nf;++ i)
    {
        fAttriVec[fs[i]] = _FaceAttriVec();
        _FaceAttriVec& fout = fAttriVec[fs[i]];
        
        const TVertex* v0 = fs[i]->he_->headVtx_;
        const TVertex* v1 = fs[i]->he_->next_->headVtx_;
        const TVertex* v2 = fs[i]->he_->next_->next_->headVtx_;

        const Point3<REAL>& p0 = v0->pos_;
        const Point3<REAL>& p1 = v1->pos_;
        const Point3<REAL>& p2 = v2->pos_;

        fout.nml  = Triangle<REAL>::normal(p0, p1, p2);
        fout.area = fout.nml.normalize2();
        const Vector3<REAL>& nl = fout.nml;
        A(0,0) = p0.x; A(0,1) = p0.y; A(0,2) = p0.z;
        A(1,0) = p1.x; A(1,1) = p1.y; A(1,2) = p1.z;
        A(2,0) = p2.x; A(2,1) = p2.y; A(2,2) = p2.z;
        A(3,0) = nl.x; A(3,1) = nl.y; A(3,2) = nl.z;

        Eigen::FullPivHouseholderQR< Eigen::Matrix<REAL,4,4> > qr = A.fullPivHouseholderQr();
        //Eigen::FullPivLU< Eigen::Matrix<REAL,4,4> > qr = A.fullPivLU();

        // solve for the three component of 
        for(int j = 0;j < 3;++ j)
        {
            b(0) = v0->data_[j];
            b(1) = v1->data_[j];
            b(2) = v2->data_[j];
            fout.av[j] = qr.solve(b);
        }
    }
}

template <class THEMesh>
void HoppeMeasuerer<THEMesh>::update_vertex_quadric(
        boost::unordered_map< TFace*, _FaceAttriVec >& fAttriVec)
{
    int nv;
    TVertex* const* vs = mesh_->vertices(nv);

    _VtxQuadric vqnow;
    vqnow.Q.setZero();
    Eigen::Matrix<REAL, 3, 3> tmat;

    for(int i = 0;i < nv;++ i)
    {
        vQuad_[vs[i]] = _VtxQuadric();
        _VtxQuadric& qout = vQuad_[vs[i]];

        THalfEdge* he0    = vs[i]->he_;
        THalfEdge* lastHe = he0;
        THalfEdge* h      = he0->flip_->next_;

        do
        {
            TFace* face = h->face_;
            assert(fAttriVec.count(face));
            const _FaceAttriVec& frec = fAttriVec[face];
            const REAL d = frec.nml.dot(vs[i]->pos_);    // n^T p

            Eigen::Matrix<REAL,3,1> nml(frec.nml.x, frec.nml.y, frec.nml.z);
            TQuadMat& Q = vqnow.Q;
            TQuadVec& b = vqnow.b;
            const Eigen::Matrix<REAL, 4, 1>& av0 = frec.av[0];
            const Eigen::Matrix<REAL, 4, 1>& av1 = frec.av[1];
            const Eigen::Matrix<REAL, 4, 1>& av2 = frec.av[2];

            Q(3,3) = HOPPE_ATTR_WEIGHT; 
            Q(4,4) = HOPPE_ATTR_WEIGHT; 
            Q(5,5) = HOPPE_ATTR_WEIGHT;
            Q.topLeftCorner<3,3>() = nml * nml.transpose();
            b.head<3>() = nml*d;

            tmat = (av0.head<3>()*av0.head<3>().transpose()) +
                   (av1.head<3>()*av1.head<3>().transpose()) +
                   (av2.head<3>()*av2.head<3>().transpose()); 
            Q.topLeftCorner<3,3>() += tmat*HOPPE_ATTR_WEIGHT;
            Q.block<3,1>(0,3) = -av0.head<3>();
            Q.block<3,1>(0,4) = -av1.head<3>();
            Q.block<3,1>(0,5) = -av2.head<3>();
            Q.topRightCorner<3,3>() *= HOPPE_ATTR_WEIGHT;
            Q.bottomLeftCorner<3,3>() = Q.topRightCorner<3,3>().transpose();
            Q *= frec.area;

            b.head<3>() += (av0.head<3>()*(-av0(3))-
                            av1.head<3>()*  av1(3) -
                            av2.head<3>()*  av2(3))*HOPPE_ATTR_WEIGHT;
            b(3) = HOPPE_ATTR_WEIGHT*av0(3);
            b(4) = HOPPE_ATTR_WEIGHT*av1(3);
            b(5) = HOPPE_ATTR_WEIGHT*av2(3);
            b *= frec.area;

            vqnow.d = ( M_SQR(d) + HOPPE_ATTR_WEIGHT * 
                       (M_SQR(av0(3)) + 
                        M_SQR(av1(3)) + 
                        M_SQR(av2(3))) )*frec.area;

            qout += vqnow;

            lastHe = h;
            h = h->flip_->next_;
        } while ( lastHe != he0 );
    }
}

template <class THEMesh>
REAL HoppeMeasuerer<THEMesh>::collapse_cost(
        TEdge* e, Point3<REAL>& v, Vector3<REAL>& u)
{
    assert( vQuad_.count(e->he_->headVtx_) && 
            vQuad_.count(e->he_->tailVtx_) );

    _VtxQuadric m = vQuad_[e->he_->headVtx_] + 
                    vQuad_[e->he_->tailVtx_];

#if 0
    TQuadVec r = m.Q.ldlt().solve(m.b);

    v.x = r(0); v.y = r(1); v.z = r(2);
    u.x = r(3); u.y = r(4); u.z = r(5);
    return r.dot(m.Q*r) - 2.*r.dot(m.b) + m.d;
#else
    // ----- add the constraint to preserve volume -----
    // construct g_vol and d_vol for volume preservation
    const TVertex* v0 = e->he_->headVtx_;
    const TVertex* v1 = e->he_->tailVtx_;
    Vector3<REAL> gvol;
    REAL dvol = 0;

    THalfEdge* h0 = e->he_->flip_;
    THalfEdge* lastHe = h0;
    for(THalfEdge* h = e->he_->next_; h != h0;
        h = lastHe->flip_->next_)
    {
        // (v1-v0)X(v2-v0)
        Vector3<REAL> nml = Triangle<REAL>::normal(
                v0->pos_, h->headVtx_->pos_, lastHe->headVtx_->pos_);
        dvol += nml.dot(v0->pos_);
        gvol += nml;
        lastHe = h;
    }

    h0 = e->he_;
    lastHe = h0;
    for(THalfEdge* h = lastHe->flip_->next_;h != h0;
        h = lastHe->flip_->next_)
    {
        // (v1-v0)X(v2-v0)
        Vector3<REAL> nml = Triangle<REAL>::normal(
                v1->pos_, h->headVtx_->pos_, lastHe->headVtx_->pos_);
        dvol += nml.dot(v1->pos_);
        gvol += nml;
        lastHe = h;
    }

    A_.topLeftCorner<6,6>() = m.Q;
    A_(0,6) = gvol.x; A_(1,6) = gvol.y; A_(2,6) = gvol.z;
    A_(6,0) = gvol.x; A_(6,1) = gvol.y; A_(6,2) = gvol.z;
    b_.head<6>() = m.b;
    b_(6) = dvol;

    x_ = A_.ldlt().solve(b_);

    v.x = x_(0); v.y = x_(1); v.z = x_(2);
    u.x = x_(3); u.y = x_(4); u.z = x_(5);
    return x_.head<6>().dot(m.Q*x_.head<6>()) - 2.*m.b.dot(x_.head<6>()) + m.d;
#endif

}

#endif
