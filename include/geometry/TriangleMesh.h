#ifndef __objwriter_h__
#define __objwriter_h__

#include <vector>
#include <string>



class Face {
public:
 Face() : v{0,0,0}, color(0) {}
 Face(size_t v0, size_t v1, size_t v2) : v{v0, v1, v2} , color(0) {}

  // Integer indices into an array of vertices
  size_t v[3];
  size_t color;
};


struct Vertex {
public:
Vertex() : x{0,0,0}, color(0) {}

  double x[3];

  // This list of pointers to incident triangles allows us 
  // to find connected components and therefore traverse a mesh.
  std::vector<Face *> faces;
  size_t color;
};



class TriangleMesh {

 public:
  TriangleMesh() {}

  // This method issues the callback to marching cubes,
  // for each cell in the grid. It populates the mesh data structure
  // by calling addTriangle() as it runs marching cubes.  
  void triangulateImplicitFunc(double xmin, double xmax,
			       double ymin, double ymax,
			       double zmin, double zmax,
			       double dx,
			       double (*fnPtr)(double, double, double));

  void print() const;
  void write(const std::string &filename) const;
  void readObj(const std::string &filename);
  double volume() const;
  void color();

  std::vector<TriangleMesh> *splitMeshes();


 private:
  std::vector<Vertex> m_verts;
  std::vector<Face *> m_faces;

  size_t _numColors;

  void addTriangle(Vertex &v1, Vertex &v2, Vertex &v3);
  size_t insertVertex(Vertex &v);
  double signedVolumeOfTriangle(const Vertex &v1, const Vertex &v2, const Vertex &v3) const;

  // helper functions
  Vertex &getVertex(size_t faceIndex, size_t vertIndex); 
  void colorNeighbors(size_t vertIndex, int color);

};



#endif
