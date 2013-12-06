#ifndef HALF_EDGE_INC
#   define HALF_EDGE_INC

#include "generic/null_type.hpp"
#include "Vertex.hpp"
#include "Edge.hpp"
#include "Face.hpp"

template <class TVData = sploosh::NullType,
          class TEData = sploosh::NullType,
          class TFData = sploosh::NullType>
class HalfEdge
{
    public:
        typedef HalfEdge<TVData, TEData, TFData>    TSelf;
        typedef TVData                              TVtxData;
        typedef TEData                              TEdgeData;
        typedef TFData                              TFaceData;
        typedef Vertex<TSelf>                       TVertex;
        typedef Face<TSelf>                         TFace;
        typedef Edge<TSelf>                         TEdge;

        int         id_;
        // pointer to the next halfedge around the current face
        TSelf*      next_;
        // pointer to the halfedge along the opposite direction
        TSelf*      flip_;

        // start and end vertices
        TVertex*    headVtx_;
        TVertex*    tailVtx_;

        TFace*      face_;

        TEdge*      edge_;

        // ----------------------------------------------------

        HalfEdge(int id):id_(id), next_(NULL), flip_(NULL), headVtx_(NULL), 
                         tailVtx_(NULL), face_(NULL), edge_(NULL)
        { }
};

#endif
