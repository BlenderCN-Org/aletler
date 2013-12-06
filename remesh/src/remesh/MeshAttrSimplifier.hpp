#ifndef MESH_ATTR_SIMPLIFIER_INC
#   define MESH_ATTR_SIMPLIFIER_INC

#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include "utils/term_msg.h"
#include "EdgeCollapsibleTriMesh.hpp"
#include "utils/PriorityQueue.hpp"
#include "utils/boost_macros.h"

/*
 * Implement the simplification algorithm
 *
 * TCollapseChecker: provides the functions to pre-check and post-check edge collapse
 *     it must provide the following functions
 *     - init(...) initialize the checker
 *     - pass_pre_check(...)
 *     - pass_post_check(...)
 * 
 * TCollapseMeasure: returns the cost of collapse a given edge
 *     it must provide the following functions
 *     - init(...) initialize the measurer
 *     - collapse_cost(...)
 *     - add_vertex / remove_vertex
 *
 * Algorithm:
 * 1. foreach edge:
 *      if it passes pre-check
 *        evaluate collapse cost v  ( collapse_cost(...) )
 *        add it into priority queue
 * 2. while P.Q. is not empty
 *      pop it with the least cost
 *      if it is not valid (e.g. the edge has been deleted) 
 *        continue
 *      if it passes post-check (e.g. triangle fairness)
 *        collapse the edge
 *        notify the TCollapseChecker
 *        deal with the newly created edges
 */
template<class TCollapseChecker, class TCollapseMeasure>
class MeshAttrSimplifier
{
    public:
        typedef EdgeCollapsibleTriMesh< Vector3<REAL>, uint32_t > THEMesh;

        MeshAttrSimplifier(const TriangleMesh<REAL>* mesh, 
                           TCollapseChecker* checker, 
                           TCollapseMeasure* measure):
                           mesh_(NULL), triMesh_(mesh), checker_(checker), measure_(measure)
        { 
            Point3<REAL> low, up;
            mesh->bounding_box(low, up);
            meshScale_ = (REAL)1. / fmax(fmax(up.x-low.x, up.y-low.y), up.z-low.z);
        }

        /*
         * u is a given vertex attribute (e.g., color)
         */
        void simplify(const Vector3<REAL>* u);

        const THEMesh* collapsible_mesh() const
        {   return mesh_; }

    private:
        typedef THEMesh::TVertex    TVertex;
        typedef THEMesh::TEdge      TEdge;
        typedef THEMesh::THalfEdge  THalfEdge;

        /* extra mesh simplification stop condition */
        bool stop();

        void init_attr_data(const Vector3<REAL>* u);

        void collapse_edge(TEdge* e, TVertex* v, uint32_t& idcnt);

    private:
        struct _EdgeRec
        {
            int             qIdx;
            THEMesh::TEdge* edge;
            REAL            cost;

            Point3<REAL>    v;   // the merged position solved while evaluating the cost
            Vector3<REAL>   u;   // the resulting attribute solved while evaluating the cost
            uint32_t        uid; // this uid should be the same as the edge->data_. It's used to
                                 // ...

            _EdgeRec(THEMesh::TEdge* e):edge(e) { }

            /* less operator for P.Q. */
            bool operator < (const _EdgeRec& b) const
            {   return cost < b.cost; }
        };

        THEMesh*                    mesh_;
        const TriangleMesh<REAL>*   triMesh_;

        REAL                        meshScale_;
        REAL                        modeScale_;
        REAL                        minCost_;

        PriorityQueue<_EdgeRec>                     que_;       // priority queue for mesh simplification
        boost::unordered_map<uint32_t, _EdgeRec*>   validES_;   // map an edge ID to an edge record in the queue
        TCollapseChecker*                           checker_;
        TCollapseMeasure*                           measure_;
};

// ----------------------------------------------------------------------------

/*
 * 1. Create the collapsible mesh
 * 2. Scale the mesh
 * 3. Assign the attribute data to each vertex
 * 3. Simplification (see above)
 */
template<class TCollapseChecker, class TCollapseMeasure>
void MeshAttrSimplifier<TCollapseChecker, TCollapseMeasure>::
simplify(const Vector3<REAL>* u)
{
    // --- create the collapsible mesh ---
    // NOTE: vertex attribute data has not yet assigned
    if ( mesh_ ) delete mesh_;
    mesh_ = new THEMesh(triMesh_);

    // --- scale the mesh and attributes ---
    mesh_->scale(meshScale_);
    init_attr_data(u);

    // -------------------------------------
    checker_->init(mesh_);
    measure_->init(mesh_);

    // --- initialize the priority queue ---
    uint32_t idcnt = 0;
    que_.clear();
    validES_.clear();
    int ne;
    THEMesh::TEdge* const* es = mesh_->edges(ne);

    que_.resize(ne*4);  // allocate enough space
    for(int i = 0;i < ne;++ i)
    {
        if ( checker_->pass_pre_check(es[i]) )
        {   // add the edge into the P.Q.
            es[i]->data_ = ++ idcnt;    // assign an edge Id
            _EdgeRec* rec = new _EdgeRec(es[i]);
            rec->cost = measure_->collapse_cost(es[i], rec->v, rec->u);
            rec->uid  = es[i]->data_;
            que_.push(rec);
            validES_[es[i]->data_] = rec;
        }
    }
    minCost_ = que_.peek()->cost;
    PRINT_MSG("min edge collapse cost: %lf\n", (double)minCost_);

    // now start to simplify the mesh
    while ( !que_.empty() && !stop() )
    {
        _EdgeRec* erec = que_.pop();

        /* If this edge has been deleted, ignore it */
        if ( !validES_.count(erec->uid) ) 
        {
            delete erec;
            continue;
        }

        assert(erec->edge->data_ == erec->uid);
        if ( checker_->pass_post_check(erec->edge, &(erec->v)) )
        {
            // collapse the edge
            TVertex* newVtx = new THEMesh::TVertex(-1);
            newVtx->pos_  = erec->v;
            newVtx->data_ = erec->u; 

            collapse_edge(erec->edge, newVtx, idcnt);
        }
        else
        {
            validES_.erase(erec->uid);
        }
        delete erec;
    }

    // --- clean the queue ---
    while ( !que_.empty() ) 
    {
        _EdgeRec* ptr = que_.pop();
        delete ptr;
    }

    mesh_->scale(1./meshScale_);
}

template<class TCollapseChecker, class TCollapseMeasure>
void MeshAttrSimplifier<TCollapseChecker, TCollapseMeasure>::
collapse_edge(TEdge* e, TVertex* v, uint32_t& idcnt)
{
    // store the two vertices that are connected to the newly created edges
    TVertex* va = e->he_->next_->headVtx_;
    TVertex* vb = e->he_->flip_->next_->headVtx_;

    validES_.erase(e->data_);
    validES_.erase(e->he_->next_->edge_->data_);
    validES_.erase(e->he_->next_->next_->edge_->data_);
    validES_.erase(e->he_->flip_->next_->edge_->data_);
    validES_.erase(e->he_->flip_->next_->next_->edge_->data_);

    measure_->edge_collapsed(e->he_->headVtx_, e->he_->tailVtx_, v);

    // collapse the mesh
    mesh_->collapse_edge(e, v);

    // update the affected edges and newly created edges
    boost::unordered_set<TEdge*> es;

    THEMesh::THalfEdge* he0 = v->he_;
    THEMesh::THalfEdge* ch  = he0;
    do
    {
        const TVertex* cvtx = ch->headVtx_;
        THEMesh::THalfEdge* che0 = cvtx->he_;
        THEMesh::THalfEdge* cch  = che0;
        do
        {
            es.insert(cch->edge_);
            cch = cch->flip_->next_;
        } while (cch != che0);

        ch = ch->flip_->next_;
    } while(ch != he0);

    foreach(TEdge* newe, es)
    {
        if ( (newe->he_->headVtx_ == v && (newe->he_->tailVtx_ == va || newe->he_->tailVtx_ == vb)) ||
             (newe->he_->tailVtx_ == v && (newe->he_->headVtx_ == va || newe->he_->headVtx_ == vb)) )
        {   // now newe is the newly created edge
            if ( checker_->pass_pre_check(newe) )
            {
                newe->data_ = ++ idcnt;
                _EdgeRec* rec = new _EdgeRec(newe);
                rec->cost = measure_->collapse_cost(newe, rec->v, rec->u);
                rec->uid  = newe->data_;
                que_.push(rec);
                validES_[newe->data_] = rec;
            }
        }
        else
        {
            if ( checker_->pass_pre_check(newe) )
            {
                if ( validES_.count(newe->data_) )
                {
                    _EdgeRec* rec = validES_[newe->data_];
                    rec->cost = measure_->collapse_cost(newe, rec->v, rec->u);
                    que_.update_node(rec);
                }
                else
                {   // not added into the queue before
                    newe->data_ = ++ idcnt;
                    _EdgeRec* rec = new _EdgeRec(newe);
                    rec->cost = measure_->collapse_cost(newe, rec->v, rec->u);
                    rec->uid  = newe->data_;
                    que_.push(rec);
                    validES_[newe->data_] = rec;
                }
            }
            else
            {
                validES_.erase(newe->data_);
            }
        }
    }
}

template<class TCollapseChecker, class TCollapseMeasure>
bool MeshAttrSimplifier<TCollapseChecker, TCollapseMeasure>::stop()
{
    PRINT_MSG("Current mesh size: %d faces\n", mesh_->num_faces());
    return (int)triMesh_->num_triangles() > mesh_->num_faces()*15;
}

/*
 * scale the attributes such that they have bounding box size 1
 */
template<class TCollapseChecker, class TCollapseMeasure>
void MeshAttrSimplifier<TCollapseChecker, TCollapseMeasure>::
init_attr_data(const Vector3<REAL>* u)
{
    const REAL inf = std::numeric_limits<REAL>::infinity();
    Vector3<REAL> low(inf, inf, inf), up(-inf, -inf, -inf);

    for(size_t i = 0;i < triMesh_->num_vertices();++ i)
    {
        low.x = fmin(low.x, u[i].x);
        low.y = fmin(low.y, u[i].y);
        low.z = fmin(low.z, u[i].z);

        up.x  = fmax(up.x, u[i].x);
        up.y  = fmax(up.y, u[i].y);
        up.z  = fmax(up.z, u[i].z);
    }
    const REAL dd = fmax(fmax(up.x-low.x, up.y-low.y), up.z-low.z);
    modeScale_ = dd < 1E-9 ? (REAL)1 : (REAL)1. / dd;

    int nv;
    THEMesh::TVertex* const* vs = mesh_->vertices(nv);
    assert(nv == (int)triMesh_->num_vertices());
    for(int i = 0;i < nv;++ i)
        vs[i]->data_.set(u[i].x*modeScale_, u[i].y*modeScale_, u[i].z*modeScale_);
}

#endif
