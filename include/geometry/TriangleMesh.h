#ifndef __objwriter_h__
#define __objwriter_h__

#include <vector>
#include <string>

#include <geometry/BoundingBox.h>
#include <sound/util.h>
#include <geometry/Triangle.h>
#include <Eigen/Dense>

using Eigen::Vector3d;
using Eigen::VectorXd;



enum MeshFileFormat {
  MFF_OBJ,
  MFF_STL,
  MFF_FASTBEM
};



// Although a Face is a triangle, this representation
// is compact and simply stores 3 integer indices
// into an array. This only has meaning as part of the
// TriangleMesh class.
class Face {
public:
  Face();
  Face(size_t v0, size_t v1, size_t v2);

  // Integer indices into an array of vertices
  size_t v[3];
  size_t color;
  
  FluidBoundaryType boundaryType;
  
  
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
  TriangleMesh() {
    _flipNormals = false;
  }

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
  bool read(const std::string &filename, MeshFileFormat mff);
  double volume() const;
  void color();
  
  void writeFastBEM(const std::string &filename,
                    const VectorXd &neumannBC) const;

  void jitter(const Vector3d &j) {
    for (size_t i = 0; i < m_verts.size(); i++) {
      Eigen::Vector3d dv(random_double(-j.x(), j.x()),
                         random_double(-j.y(), j.y()),
                         random_double(-j.z(), j.z()));
      m_verts[i].x += dv;
    }
  }
  
  std::vector<TriangleMesh> *splitMeshes();

  double surfaceArea();
  
  void clearAll() {
    m_verts.clear();
    for (size_t i = 0; i < m_faces.size(); i++) {
      delete m_faces[i];
      m_faces[i] = NULL;
    }
    m_faces.clear();
  }

  // TODO: deprecate this
  void boundingBox(double boxmin[3], double boxmax[3]) const;

  
 
  // newer version
  void getBoundingBox(BoundingBox &box) const {

    double boxmin[3];
    double boxmax[3];

    boundingBox(boxmin, boxmax);
    
    Vector3d bmin(boxmin[0], boxmin[1], boxmin[2]);
    Vector3d bmax(boxmax[0], boxmax[1], boxmax[2]);

    box.set(bmin, bmax);
  }

  // Returns the normal vector for triangle i in the list of triangle faces.
  Vector3d normal(size_t i);

  // returns the ith triangle in the list of faces
  Triangle triangle(size_t i) const {
    Triangle t;

    Face *f = m_faces[i];
    
    if (_flipNormals) {
      t.a = m_verts[f->v[0]].x;
      t.b = m_verts[f->v[2]].x;
      t.c = m_verts[f->v[1]].x;


    } else {
      t.a = m_verts[f->v[0]].x;
      t.b = m_verts[f->v[1]].x;
      t.c = m_verts[f->v[2]].x;
    }
    
    return t;
  }

  size_t size() const {
    return m_faces.size();
  }
  
  void flipNormals() {
    _flipNormals = true;
  }
  
  Eigen::VectorXd &triangleAreas() { return _triangleAreas; }


  void translate(const Vector3d &dv) {
    for (size_t i = 0; i < m_verts.size(); i++) {
      m_verts[i].x += dv;
    }
  }
  
  void scale(const Vector3d &ds) {
    // TODO: a proper scaling would translate the mesh in order to
    // center the bounding box of the object, scale, and then translate back
    // For now I'm just writing this to generate some fake meshes
    for (size_t i = 0; i < m_verts.size(); i++) {
      m_verts[i].x = m_verts[i].x.cwiseProduct(ds);
    }
  }
  
  // This is just a state variable that determines the type of NEW triangles
  // to be added to the mesh:
  FluidBoundaryType boundaryType;
  
  
  
 private:
  std::vector<Vertex> m_verts;
  std::vector<Face *> m_faces;

  size_t _numColors;
  
  bool _flipNormals;
  
  

  
  double triangleArea(size_t i) { return _triangleAreas(i); }
  VectorXd _triangleAreas;
  
  void addTriangle(Vertex &v1, Vertex &v2, Vertex &v3);
  size_t insertVertex(Vertex &v);
  double signedVolumeOfTriangle(const Vertex &v1, const Vertex &v2, const Vertex &v3) const;

  // helper functions
  Vertex &getVertex(size_t faceIndex, size_t vertIndex); 
  void colorNeighbors(size_t vertIndex, size_t color);

  void writeObj(const std::string &filename) const;
  bool readObj(const std::string &filename);
  bool readStl(const std::string &filename);
  
};



#endif
