#ifndef __objwriter_h__
#define __objwriter_h__

#include <vector>
#include <string>

#include <geometry/BoundingBox.h>

#include <Eigen/Dense>

using Eigen::Vector3d;

enum MeshFileFormat {
  MFF_OBJ
};
  

// A Triangle actually contains 3 points and can stand
// on its own (outside of a TriangleMesh object)
class Triangle {
 public:

  // Assume counterclockwise winding order
  Vector3d a, b, c;

  Vector3d normal() {
    return (b-a).cross(c-a);
  }

  // area of parallelogram
  double area_pgram() {
    return normal().dot(normal().normalized());
  }

  // area of triangle
  double area() {
    return 0.5 * area_pgram();
  }

  double potential(const Vector3d &p) {
    
    return fabs (fn_I_qrg(a,b,p) +
		 fn_I_qrg(b,c,p) +
		 fn_I_qrg(c,a,p));
  }

  Vector3d centroid() {
    return (a + b + c) / 3.0;
  }

 private:
  
      
  // One term in the potential at point P
  // H. Wang et al, Harmonic Parameterization by Electrostatics
  // (Eqn 15)
  double fn_I_qrg(const Vector3d &q, 
		  const Vector3d &r,
		  const Vector3d &p) {
    Vector3d n = ((c-a).cross(b-a)).normalized();

    double h = n.dot(p-a);
    Vector3d g = p - h * n;

    Vector3d rq = q-r;
    Vector3d rg = g-r;
    Vector3d rp = p-r;
    
    Vector3d qr = r-q;
    Vector3d qg = g-q;
    Vector3d qp = p-q;
    
    double sig = (qg.cross(rg)).dot(n);
    double N = rq.dot(rp) + rq.norm() * rp.norm();
    double D = rq.dot(qp) + rq.norm() * qp.norm();


    // TODO: check against double precision limits
    if (sig == 0 || N == 0 || D == 0) {
      return 0;
    }
    
    double first_term = (sig * log(N/D)) / rq.norm();
    
    double N2 = sig * rq.dot(rg) * (fabs(h) - rp.norm());
    double D2 = sig*sig * rp.norm() + fabs(h) * pow(rq.dot(rg), 2);
    double second_term = fabs(h) * atan2(N2, D2);

    double N3 = sig * qr.dot(qg) * (fabs(h) - qp.norm());
    double D3 = sig*sig * qp.norm() + fabs(h) * pow(qr.dot(qg), 2);
    double third_term = fabs(h) * atan2(N3, D3);

    return first_term + second_term + third_term;
  }
  

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
    t.a = m_verts[f->v[0]].x;
    t.b = m_verts[f->v[1]].x;
    t.c = m_verts[f->v[2]].x;

    return t;
  }

  size_t size() const {
    return m_faces.size();
  }

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
