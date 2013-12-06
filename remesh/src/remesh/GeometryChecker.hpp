#ifndef GEOMETRY_CHECKER_INC
#   define GEOMETRY_CHECKER_INC

#include <boost/unordered_set.hpp>
#include "remesh_param.h"

template <class THEMesh>
class GeometryChecker
{
    public:
        typedef typename THEMesh::TEdge     TEdge;
        typedef typename THEMesh::TVertex   TVertex;
        typedef typename THEMesh::THalfEdge THalfEdge;

        void init(THEMesh* mesh)
        {
            assert(mesh);
            mesh_ = mesh;
        }

        bool pass_pre_check(const TEdge* e) const;
       
        /*
         * called right before the collapse operation
         */
        bool pass_post_check(const TEdge* e, const Point3<REAL>* v) const;

    private:
        /*
         * return true if deleting e will result in a non-manifold mesh
         */
        bool will_degenerate(const TEdge* e) const;

        bool check_fairness(const TEdge* e, const Point3<REAL>* v) const;

    private:
        THEMesh*    mesh_;
};

REAL tri_fairness(const Point3<REAL>& p0, const Point3<REAL>& p1, const Point3<REAL>& p2)
{
    return 4.*sqrt(3.)*Triangle<REAL>::area(p0, p1, p2) / 
           (p0.distance_sqr(p1)+p1.distance_sqr(p2)+p0.distance_sqr(p2));
}

// -------------------------------------------------------------

template <class THEMesh>
bool GeometryChecker<THEMesh>::pass_pre_check(const TEdge* e) const
{
    if ( will_degenerate(e) ) return false;
    return true;
}

template <class THEMesh>
bool GeometryChecker<THEMesh>::will_degenerate(const TEdge* e) const
{
    const TVertex* v0 = e->he_->tailVtx_;
    const TVertex* va = e->he_->next_->headVtx_;
    const TVertex* vb = e->he_->flip_->next_->headVtx_;

    boost::unordered_set<TVertex*> ss;

    for(THalfEdge* ch = e->he_->next_;ch != e->he_->flip_;ch = ch->flip_->next_)
        if ( ch->headVtx_ != v0 && ch->headVtx_ != va && ch->headVtx_ != vb )
            ss.insert(ch->headVtx_);

    for(THalfEdge* ch = e->he_->flip_->next_;ch != e->he_;ch = ch->flip_->next_)
        if ( ss.count(ch->headVtx_) ) return true;
    return false;
}

template <class THEMesh>
bool GeometryChecker<THEMesh>::pass_post_check(
        const TEdge* e, const Point3<REAL>* v) const
{
    if ( will_degenerate(e) || !check_fairness(e, v) ) return false;
    return true;
}

/*
 * Make sure the worst fairness of the resulting triangle mesh is 
 * above a threshold
 */
template <class THEMesh>
bool GeometryChecker<THEMesh>::check_fairness(
        const TEdge* e, const Point3<REAL>* v) const
{
    const TVertex* v0 = e->he_->headVtx_;
    const TVertex* v1 = e->he_->tailVtx_;

    THalfEdge* h0     = e->he_;
    THalfEdge* lastHe = h0->next_;
    for(THalfEdge* h = lastHe->flip_->next_;
        h != h0->flip_; h = lastHe->flip_->next_)
    {
        if ( tri_fairness(*v, h->headVtx_->pos_, lastHe->headVtx_->pos_) < 
                TRI_FAIRNESS_MIN_THRESH ) return false;
        lastHe = h;
    }

    h0 = h0->flip_;
    lastHe = h0->next_;
    for(THalfEdge* h = lastHe->flip_->next_;
        h != h0->flip_; h = lastHe->flip_->next_)
    {
        if ( tri_fairness(*v, h->headVtx_->pos_, lastHe->headVtx_->pos_) < 
                TRI_FAIRNESS_MIN_THRESH ) return false;
        lastHe = h;
    }
    return true;
}

#endif
