#ifndef EDGE_INC
#   define EDGE_INC

template < class THalfEdge >
class Edge
{
    public:
        typedef typename THalfEdge::TEdgeData   TData;

        int         id_;
        // pointer to one of the halfedge
        THalfEdge*  he_;
        // associated data structure
        TData       data_;

        Edge(int id):id_(id), he_(NULL) { }
        Edge(int id, THalfEdge* h):id_(id), he_(h) { }
};

#endif

