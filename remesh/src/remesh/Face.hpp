#ifndef FACE_INC
#   define FACE_INC

#include "config.h"

template < class THalfEdge >
class Face
{
    public:
        typedef typename THalfEdge::TFaceData   TData;

        int         id_;
        // pointer to one of the halfedge
        THalfEdge*  he_;
        // associated data structure
        TData       data_;

        Face(int id):id_(id), he_(NULL) { }
        Face(int id, THalfEdge* h):id_(id), he_(h) { }
};

#endif
