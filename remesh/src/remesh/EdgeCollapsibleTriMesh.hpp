#ifndef EDGE_COLLAPSIBLE_TRI_MESH_INC
#   define EDGE_COLLAPSIBLE_TRI_MESH_INC

#include "config.h"
#include "HalfEdge.hpp"
#include "utils/macros.h"
#include "geometry/TriangleMesh.hpp"

#ifdef USE_NAMESPACE
namespace sploosh
{
#endif

template <class TVData = sploosh::NullType,
          class TEData = sploosh::NullType,
          class TFData = sploosh::NullType>
class EdgeCollapsibleTriMesh
{
    public:
        typedef EdgeCollapsibleTriMesh<TVData, TEData, TFData> TSelf;
        typedef HalfEdge<TVData, TEData, TFData>        THalfEdge;
        typedef Face<THalfEdge>                         TFace;
        typedef Edge<THalfEdge>                         TEdge;
        typedef Vertex<THalfEdge>                       TVertex;

        EdgeCollapsibleTriMesh(const TriangleMesh<REAL>* mesh);

        ~EdgeCollapsibleTriMesh()
        {
            for(int i = 0;i < nVertices_;++ i) delete vertices_[i];
            for(int i = 0;i < nFaces_;++ i) delete faces_[i];
            for(int i = 0;i < nEdges_;++ i) delete edges_[i];
            for(int i = 0;i < nHes_;++ i) delete hes_[i];
        }

        void collapse_edge(TEdge* e, TVertex* v);

        /* --- scale mesh vertices --- */
        void scale(REAL s)
        {
            for(int i = 0;i < nVertices_;++ i)
                vertices_[i]->pos_ *= s;
        }

        inline int num_faces() const
        {   return nFaces_; }

        TEdge* const* edges(int& ne) const
        {   
            ne = nEdges_;
            return edges_.data(); 
        }
        TVertex* const* vertices(int& nv) const
        {
            nv = nVertices_;
            return vertices_.data();
        }
        THalfEdge* const* half_edges(int& nh) const
        {
            nh = nHes_;
            return hes_.data();
        }
        TFace* const* faces(int& nf) const
        {
            nf = nFaces_;
            return faces_.data();
        }

#if defined(DEBUG) | defined(_DEBUG)
        void sanity_check();
#endif
    private:
        void collapse_half_tri_fan(THalfEdge* he, const TEdge* e, TVertex* v);

        void add_vertex(TVertex* v);
        void remove_vertex(TVertex* v);

        void add_edge(TEdge* e);
        void remove_edge(TEdge* e);

        void remove_face(TFace* f);

        void remove_half_edge(THalfEdge* h);

    private:
        std::vector< TVertex* >     vertices_;
        int                         nVertices_;

        std::vector< TFace* >       faces_;
        int                         nFaces_;

        std::vector< TEdge* >       edges_;
        int                         nEdges_;

        std::vector< THalfEdge* >   hes_;
        int                         nHes_;
};

///////////////////////////////////////////////////////////////////////////////

template <class TVData, class TEData, class TFData>
EdgeCollapsibleTriMesh<TVData, TEData, TFData>::
EdgeCollapsibleTriMesh(const TriangleMesh<REAL>* mesh)
{
    boost::unordered_map< std::pair<int,int>, THalfEdge*>   hmap;

    const std::vector< Point3<REAL> >& vtx = mesh->vertices();
    const std::vector< Tuple3ui >&     tri = mesh->triangles();

    // --- allocate space ---
    vertices_.resize(vtx.size());
    faces_.resize(tri.size());
    nVertices_ = (int)vtx.size();
    nFaces_    = (int)tri.size();

    // create vertex list
    for(size_t i = 0;i < vtx.size();++ i)
        vertices_[i] = new TVertex((int)i, vtx[i]);

    nHes_  = nEdges_ = 0;
    THalfEdge* he[3];
    // create each face
    for(size_t ti = 0;ti < tri.size();++ ti)
    {
        faces_[ti] = new TFace((int)ti);

        for(int j = 0;j < 3;++ j)
        {
            he[j] = new THalfEdge(nHes_++);
            hes_.push_back( he[j] );
        }
        // face
        faces_[ti]->he_ = he[0];

        for(int j = 0;j < 3;++ j)
        {
            int startVId = tri[ti][j];
            int endVId   = tri[ti][(j+1) % 3];
            std::pair<int,int> eId = startVId < endVId ? 
                        std::make_pair(startVId, endVId) :
                        std::make_pair(endVId, startVId);

            // vertex
            vertices_[startVId]->he_ = he[j];
            // halfedge data fields
            he[j]->next_    = he[(j+1) % 3];
            he[j]->tailVtx_ = vertices_[startVId];
            he[j]->headVtx_ = vertices_[endVId];
            he[j]->face_    = faces_[ti];

            // edge
            if ( !hmap.count(eId) ) // first time to traverse this edge
            {
                TEdge* e = new TEdge(nEdges_++);
                e->he_ = he[j];
                edges_.push_back(e);

                he[j]->edge_ = e;
                hmap[eId] = he[j];
            }
            else
            {                       // second time to traverse this edge
                THalfEdge* fhe = hmap[eId];

                he[j]->edge_ = fhe->edge_;
                he[j]->flip_ = fhe;
                fhe->flip_   = he[j];
            }
        }
    }

    for(int i = 0;i < nHes_;++ i)
        if ( !hes_[i]->flip_ )
        {
            SHOULD_NEVER_HAPPEN(-1);
        }

#if defined(DEBUG) | defined(_DEBUG)
    sanity_check();
#endif
}


template <class TVData, class TEData, class TFData>
void EdgeCollapsibleTriMesh<TVData, TEData, TFData>::
collapse_edge(TEdge* e, TVertex* v)
{
    assert(e->id_ >= 0 && e->id_ < nEdges_);

    THalfEdge* e1 = e->he_->next_;
    THalfEdge* e2 = e1->next_;

    THalfEdge* e3 = e->he_->flip_->next_;
    THalfEdge* e4 = e3->next_;
    assert(e1!=e3);

    collapse_half_tri_fan(e->he_->next_, e, v);
    collapse_half_tri_fan(e->he_->flip_->next_, e, v);

    e1->flip_->flip_ = e2->flip_;
    e2->flip_->flip_ = e1->flip_;
    e3->flip_->flip_ = e4->flip_;
    e4->flip_->flip_ = e3->flip_;

    e1->headVtx_->he_ = e1->flip_;
    e3->headVtx_->he_ = e3->flip_;

    TEdge* eA = new TEdge(-1, e1->flip_);
    TEdge* eB = new TEdge(-1, e3->flip_);
    e1->flip_->edge_ = e2->flip_->edge_ = eA;
    e3->flip_->edge_ = e4->flip_->edge_ = eB;
    eA->he_ = e1->flip_;
    eB->he_ = e3->flip_;
    v->he_  = e2->flip_;

    remove_vertex(e->he_->headVtx_);
    remove_vertex(e->he_->tailVtx_);
    add_vertex(v);

    remove_edge(e1->edge_);
    remove_edge(e2->edge_);
    remove_edge(e3->edge_);
    remove_edge(e4->edge_);
    add_edge(eA);
    add_edge(eB);

    remove_face(e->he_->face_);
    remove_face(e->he_->flip_->face_);

    remove_half_edge(e1);
    remove_half_edge(e2);
    remove_half_edge(e3);
    remove_half_edge(e4);
    remove_half_edge(e->he_->flip_);
    remove_half_edge(e->he_);
    remove_edge(e);
}

template <class TVData, class TEData, class TFData>
void EdgeCollapsibleTriMesh<TVData, TEData, TFData>::
collapse_half_tri_fan(THalfEdge* he, const TEdge* e, TVertex* v)
{
    while ( he->edge_ != e )
    {
        he->tailVtx_        = v;
        he->flip_->headVtx_ = v;
        he = he->flip_->next_;
    }
}

template <class TVData, class TEData, class TFData>
void EdgeCollapsibleTriMesh<TVData, TEData, TFData>::add_vertex(TVertex* v)
{
    if ( unlikely(nVertices_ == (int)vertices_.size()) )  // if the verctor is full, extend it
        vertices_.push_back(v);
    else
        vertices_[nVertices_] = v;

    v->id_ = nVertices_ ++;
}

template <class TVData, class TEData, class TFData>
void EdgeCollapsibleTriMesh<TVData, TEData, TFData>::remove_vertex(TVertex* v)
{
    assert(v->id_ >= 0 && v->id_ < nVertices_);
    assert(vertices_[v->id_] == v);

    vertices_[v->id_] = vertices_[nVertices_-1];
    vertices_[v->id_]->id_ = v->id_;
    -- nVertices_;
    delete v;
}

template <class TVData, class TEData, class TFData>
void EdgeCollapsibleTriMesh<TVData, TEData, TFData>::add_edge(TEdge* e)
{
    if ( unlikely(nEdges_ == (int)edges_.size()) )  // if the verctor is full, extend it
        edges_.push_back(e);
    else
        edges_[nEdges_] = e;

    e->id_ = nEdges_ ++;
}

template <class TVData, class TEData, class TFData>
void EdgeCollapsibleTriMesh<TVData, TEData, TFData>::remove_edge(TEdge* e)
{
    assert(e->id_ >= 0 && e->id_ < nEdges_);
    assert(edges_[e->id_] == e);

    edges_[e->id_] = edges_[nEdges_-1];
    edges_[e->id_]->id_ = e->id_;
    -- nEdges_;
    delete e;
}

template <class TVData, class TEData, class TFData>
void EdgeCollapsibleTriMesh<TVData, TEData, TFData>::remove_face(TFace* f)
{
    assert(f->id_ >= 0 && f->id_ < nFaces_);
    assert(faces_[f->id_] == f);

    faces_[f->id_] = faces_[nFaces_-1];
    faces_[f->id_]->id_ = f->id_;
    -- nFaces_;
    delete f;
}

template <class TVData, class TEData, class TFData>
void EdgeCollapsibleTriMesh<TVData, TEData, TFData>::
remove_half_edge(THalfEdge* h)
{
    assert(h->id_ >= 0 && h->id_ < nHes_);
    assert(hes_[h->id_] == h);

    hes_[h->id_] = hes_[nHes_-1];
    hes_[h->id_]->id_ = h->id_;
    -- nHes_;
    delete h;
}

#if defined(DEBUG) | defined(_DEBUG)
template <class TVData, class TEData, class TFData>
void EdgeCollapsibleTriMesh<TVData, TEData, TFData>::sanity_check()
{
    printf("# edges:   %d\n", nEdges_);
    printf("# h-edges: %d\n", nHes_);

    // sanity check
    for(int i = 0;i < nVertices_;++ i) 
    {
        assert(vertices_[i]->he_);
if ( vertices_[i]->he_->id_ >= nHes_ ) 
std::cerr << i << "  " << vertices_[i]->he_->id_ << std::endl;
        assert(vertices_[i]->he_->id_ < nHes_);
    }
    for(int i = 0;i < nEdges_;++ i)
    {
        assert(edges_[i]->he_);
        assert(edges_[i]->he_->edge_ == edges_[i]);
        assert(edges_[i]->id_ == i);
        assert(edges_[i]->he_->next_->next_ != edges_[i]->he_->flip_->next_->flip_);
    }

    for(int i = 0;i < nHes_;++ i)
    {
        assert(hes_[i]->flip_);
        assert(hes_[i]->next_);
        assert(hes_[i]->flip_->flip_ == hes_[i]);
        assert(hes_[i]->headVtx_ == hes_[i]->flip_->tailVtx_);
        assert(hes_[i]->tailVtx_ == hes_[i]->flip_->headVtx_);
        assert(hes_[i]->next_->next_->next_ == hes_[i]);
        assert(hes_[i]->id_ == i);
        assert(hes_[i]->edge_->id_ >= 0);
        assert(edges_[hes_[i]->edge_->id_] == hes_[i]->edge_);
    }
}
#endif

#ifdef USE_NAMESPACE
}
#endif
#endif
