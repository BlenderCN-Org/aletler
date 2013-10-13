#ifndef __objwriter_h__
#define __objwriter_h__

#include <vector>
#include <string>
#include <Eigen/Dense>

using Eigen::Vector3d;

enum MeshFileFormat {
  MFF_OBJ
};
  


class Face {
public:
  Face();
  Face(size_t v0, size_t v1, size_t v2);

  // Integer indices into an array of vertices
  size_t v[3];
  size_t color;
};


struct Vertex {
public:
  Vertex();
  Vector3d x;

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
  void triangulateImplicitFunc(float xmin, float xmax,
			       float ymin, float ymax,
			       float zmin, float zmax,
			       float dx,
			       double (*fnPtr)(double, double, double));

  void print() const;
  void write(const std::string &filename, MeshFileFormat mff) const;
  void read(const std::string &filename, MeshFileFormat mff);
  double volume() const;
  void color();

  std::vector<TriangleMesh> *splitMeshes();

  void clearAll() {
    m_verts.clear();
    for (size_t i = 0; i < m_faces.size(); i++) {
      delete m_faces[i];
      m_faces[i] = NULL;
    }
    m_faces.clear();
  }

  void boundingBox(double boxmin[3], double boxmax[3]);

 private:
  std::vector<Vertex> m_verts;
  std::vector<Face *> m_faces;

  size_t _numColors;

  void addTriangle(Vertex &v1, Vertex &v2, Vertex &v3);
  size_t insertVertex(Vertex &v);
  double signedVolumeOfTriangle(const Vertex &v1, const Vertex &v2, const Vertex &v3) const;

  // helper functions
  Vertex &getVertex(size_t faceIndex, size_t vertIndex); 
  void colorNeighbors(size_t vertIndex, size_t color);

  void writeObj(const std::string &filename) const;
  void readObj(const std::string &filename);
  
};



#endif
