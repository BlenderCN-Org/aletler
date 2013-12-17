#ifndef __objwriter_h__
#define __objwriter_h__

#include <vector>
#include <string>

#include <geometry/BoundingBox.h>
#include <sound/util.h>
#include <Eigen/Dense>

using Eigen::Vector3d;
using Eigen::Vector2d;
using Eigen::VectorXd;

typedef double (*scalarFn3d)(const Vector3d &yi, const Vector3d &xj, const Vector3d &nj);

enum MeshFileFormat {
  MFF_OBJ,
  MFF_STL
};

enum TriangleQuadrature {
  VERTEX,
  STRANG3,
  STRANG5,
  GAUSS4X4,
};

static double oneFn(const Vector3d &yi, const Vector3d &xj, const Vector3d &nj) {
  return 1.0;
}

static double neumannMatrixEntry(const Vector3d &yi, const Vector3d &xj, const Vector3d &nj) {
  
  return (xj - yi).dot(nj.normalized()) / pow( (xj - yi).norm(), 3);
}

static double dirichletMatrixEntry(const Vector3d &yi, const Vector3d &xj, const Vector3d &nj) {
  return 1.0 / (xj - yi).norm();
}


// VERTEX, order 3, degree precision 1
static const Vector2d vertex_abscissas[3] = {
  Vector2d(1.00000000000000000000,  0.00000000000000000000),
  Vector2d(0.00000000000000000000,  1.00000000000000000000),
  Vector2d(0.00000000000000000000,  0.00000000000000000000)
};


static const double vertex_weights[3] = {
  0.33333333333333333333,
  0.33333333333333333333,
  0.33333333333333333333
};


// STRANG3, order 4, degree of precision 3
static const Vector2d strang3_abscissas[4] = {
  Vector2d(0.33333333333333333333,  0.33333333333333333333),
  Vector2d(0.60000000000000000000,  0.20000000000000000000),
  Vector2d(0.20000000000000000000,  0.60000000000000000000),
  Vector2d(0.20000000000000000000,  0.20000000000000000000)
};

static const double strang3_weights[4] = {
  -0.56250000000000000000,
  0.52083333333333333333,
  0.52083333333333333333,
  0.52083333333333333333
};

// STRANG5, order 6, dop 4
static const Vector2d strang5_abscissas[6] = {
  Vector2d(0.816847572980459,  0.091576213509771),
  Vector2d(0.091576213509771,  0.816847572980459),
  Vector2d(0.091576213509771,  0.091576213509771),
  Vector2d(0.108103018168070,  0.445948490915965),
  Vector2d(0.445948490915965,  0.108103018168070),
  Vector2d(0.445948490915965,  0.445948490915965)
};

static const double strang5_weights[6] = {
  0.109951743655322,
  0.109951743655322,
  0.109951743655322,
  0.223381589678011,
  0.223381589678011,
  0.223381589678011
};


// http://people.sc.fsu.edu/~%20jburkardt/datasets/quadrature_rules_tri/quadrature_rules_tri.html
// GAUSS4X4, order 16, degree of precision 7, (essentially a product of two 4 point 1D Gauss-Legendre rules).
static const Vector2d gauss4x4_abscissas[16] = {
  Vector2d(0.0571041961,  0.06546699455602246),
  Vector2d(0.2768430136 , 0.05021012321401679),
  Vector2d(0.5835904324 , 0.02891208422223085),
  Vector2d(0.8602401357 , 0.009703785123906346),
  Vector2d(0.0571041961 , 0.3111645522491480),
  Vector2d(0.2768430136 , 0.2386486597440242),
  Vector2d(0.5835904324 , 0.1374191041243166),
  Vector2d(0.8602401357 , 0.04612207989200404),
  Vector2d(0.0571041961 , 0.6317312516508520),
  Vector2d(0.2768430136 , 0.4845083266559759),
  Vector2d(0.5835904324 , 0.2789904634756834),
  Vector2d(0.8602401357 , 0.09363778440799593),
  Vector2d(0.0571041961 , 0.8774288093439775),
  Vector2d(0.2768430136 , 0.6729468631859832),
  Vector2d(0.5835904324 , 0.3874974833777692),
  Vector2d(0.8602401357 , 0.1300560791760936)};

static const double gauss4x4_weights[16] = {
  0.04713673637581137,
  0.07077613579259895,
  0.04516809856187617,
  0.01084645180365496,
  0.08837017702418863,
  0.1326884322074010,
  0.08467944903812383,
  0.02033451909634504,
  0.08837017702418863,
  0.1326884322074010,
  0.08467944903812383,
  0.02033451909634504,
  0.04713673637581137,
  0.07077613579259895,
  0.04516809856187617,
  0.01084645180365496};


// A Triangle actually contains 3 points and can stand
// on its own (outside of a TriangleMesh object)
class Triangle {
 public:

  // Assume counterclockwise winding order
  Vector3d a, b, c;

  Vector3d normal() const {
    return (b-a).cross(c-a);
  }

  // area of parallelogram
  double area_pgram() const {
    return fabs(normal().dot(normal().normalized()));
  }

  // area of triangle
  double area() const {
    return 0.5 * area_pgram();
  }

  double potential(const Vector3d &p) {


    return fabs (fn_I_qrg(a,b,p) +
		 fn_I_qrg(b,c,p) +
		 fn_I_qrg(c,a,p));
    /*
    return fabs (goto_I_qrg(a,b,p) +
		 goto_I_qrg(b,c,p) +
		 goto_I_qrg(c,a,p));
    */
  }

  Vector3d centroid() {
    return (a + b + c) / 3.0;
  }

  
  // Maps point p on the 2d unit triangle ( (0,0), (1,0), (0,1) )
  // to a point on this triangle, in 3d
  Vector3d mapUnitTriangle(const Vector2d &p) const {
    
    // barycentric coords
    double ell1, ell2, ell3;
    ell2 = p.x();
    ell3 = p.y();
    ell1 = 1 - ell2 - ell3;
    
    return (ell1 * a) + (ell2 * b) + (ell3 * c);
    
  }
  
  
  
  double integral(scalarFn3d fn,
                  const Vector3d &refpt,
                  TriangleQuadrature quadtype = GAUSS4X4) const {
    
    
    int quad_num = 0;
    const double *quad_weights;
    const Vector2d *quad_abscissas;
    
    if (quadtype == GAUSS4X4) {
      quad_weights = &gauss4x4_weights[0];
      quad_abscissas = &gauss4x4_abscissas[0];
      quad_num = 16;
    } else if (quadtype == STRANG3) {
      quad_weights = &strang3_weights[0];
      quad_abscissas = &strang3_abscissas[0];
      quad_num = 4;
    } else if (quadtype == STRANG5) {
      quad_weights = &strang5_weights[0];
      quad_abscissas = &strang5_abscissas[0];
      quad_num = 6;
    } else if (quadtype == VERTEX) {
      quad_weights = &vertex_weights[0];
      quad_abscissas = &vertex_abscissas[0];
      quad_num = 3;
        
    } else {
      quad_weights = NULL;
      quad_abscissas = NULL;
    }
    
    double wtdsum = 0;
    
    for (size_t i = 0; i < quad_num; i++) {
      wtdsum += quad_weights[i] * fn(refpt,
                                     mapUnitTriangle(quad_abscissas[i]),
                                     normal());
    }
    
    return area() * wtdsum;
  }
  
 private:
  

  double goto_I_qrg(const Vector3d &q,
		    const Vector3d &r,
		    const Vector3d &p) {

    Vector3d n = ((a-c).cross(b-c)).normalized();
    double h = fabs(n.dot(p-a));
    Vector3d g = p - h * n;    
    double sig = ((q-g).cross(r-g)).dot(n);

    Vector3d rq = r-q; Vector3d rg = r-g; Vector3d rp = r-p;
    Vector3d qr = q-r; Vector3d qg = q-g; Vector3d qp = q-p;

    double N = rq.dot(rg) + rq.norm() * rp.norm();
    double D = -qr.dot(qg) + qr.norm() * qp.norm();
    
    if (N == 0 || D == 0) return 0;

    double first_term = (sig / rq.norm()) * log(N / D);

    double N2 = sig * rq.dot(rg) * (h - rp.norm());
    double D2 = sig*sig * rp.norm() + h * pow(rq.dot(rg), 2);

    double second_term = h * atan2(N2, D2);

    double N3 = sig * qr.dot(qg) * (h - qp.norm());
    double D3 = sig*sig * qp.norm() + h * pow(qr.dot(qg), 2);
    
    double third_term = h * atan2(N3, D3);

    return first_term + second_term + third_term;
  }

      
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

  void jitter(const Vector3d &j) {
    for (size_t i = 0; i < m_verts.size(); i++) {
      Eigen::Vector3d dv(random_double(-j.x(), j.x()),
                         random_double(-j.y(), j.y()),
                         random_double(-j.z(), j.z()));
      m_verts[i].x += dv;
    }
  }
  
  std::vector<TriangleMesh> *splitMeshes();

  double surfaceArea() {
    // This is mainly to test my integration function
    
    // ANALYTIC (SORT OF) -- just the sum of all the triangle areas
    double sa = 0;
    for (size_t i = 0; i < size(); i++) {
      Triangle t = triangle(i);
      sa += t.area();
    }
    
    std::cout << "analytic version: " << sa << std::endl;
    
    // INTEGRAL
    sa = 0;
    for (size_t i = 0; i < size(); i++) {
      Triangle t = triangle(i);
      sa += t.integral(oneFn, Vector3d(0,0,0), STRANG3);
    }
    
    std::cout << "integral version: " << sa << std::endl;
    
    return sa;
  }
  
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
