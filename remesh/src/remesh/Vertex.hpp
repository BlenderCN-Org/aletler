#ifndef VERTEX_INC
#   define VERTEX_INC

#include "config.h"
#include "geometry/Point3.hpp"

template < class THalfEdge >
class Vertex
{
    public:
        typedef typename THalfEdge::TVtxData   TData;

        int             id_;
        // vertex position
        Point3<REAL>    pos_;
        // pointer to the *outgoing* halfedge
        THalfEdge*      he_;
        // associated data structure
        TData           data_;

        Vertex(int id):id_(id), he_(NULL) { }
        Vertex(int id, const Point3<REAL>& p):id_(id), pos_(p), he_(NULL) { }
};

#endif
