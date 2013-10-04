#include "TriangleMesh.h"

#include <iostream>
#include <cmath>
#include <cassert>
#include <algorithm>
#include <fstream>
#include <iomanip> // set precision on output

#include "MarchingSource.h"


Face::Face() : v{0,0,0}, color(0) {}
Face::Face(size_t v0, size_t v1, size_t v2) : v{v0, v1, v2} , color(0) {}
Vertex::Vertex() : x{0,0,0}, color(0) {}

bool operator==(const Vertex &lhs, const Vertex &rhs);
bool operator==(const Face &lhs, const Face &rhs);
bool approxEq(double a, double b);

static double roundDouble(double d) {
  
  return floor(d * 1e6 + 0.5) / 1e6;
}

static const double TOLERANCE = 1e-6;

// Compares two floating point numbers. 
// Returns "true" if they are equal, within TOLERANCE
bool approxEq(double a, double b) {
  return fabs(a-b) < TOLERANCE;
}


// Comparators for vertices & faces
bool operator==(const Vertex &lhs, const Vertex &rhs) {
  return approxEq(lhs.x[0], rhs.x[0]) 
    && approxEq(lhs.x[1], rhs.x[1]) 
    && approxEq(lhs.x[2], rhs.x[2]);
}

bool operator==(const Face &lhs, const Face &rhs) {
  return lhs.v[0] == rhs.v[0] 
    && lhs.v[1] == rhs.v[1] 
    && lhs.v[2] == rhs.v[2];
}


// Real triangles don't have duplicate vertices!
static bool isDegenerateTriangle(const Vertex &v0, const Vertex &v1, const Vertex &v2) {
  return v0 == v1 || v1 == v2 || v0 == v2;
}


void TriangleMesh::addTriangle(Vertex &v0, Vertex &v1, Vertex &v2) {

  if (isDegenerateTriangle(v0, v1, v2)) {
    std::cout << "not adding degenerate triangle with duplicate vertices" << std::endl;
    return;
  }

  Face *tri = new Face(insertVertex(v0),
		       insertVertex(v1),
		       insertVertex(v2));
  

  // Make sure each vertex contains pointer to this triangle
  m_verts[tri->v[0]].faces.push_back(tri);
  m_verts[tri->v[1]].faces.push_back(tri);
  m_verts[tri->v[2]].faces.push_back(tri);

  m_faces.push_back(tri);
}


size_t TriangleMesh::insertVertex(Vertex &v) {
  size_t vIndex = 0;
    
    
  // is v already in m_verts? Then get index:
  std::vector<Vertex>::iterator pos = std::find(m_verts.begin(), m_verts.end(), v);
  if (pos != m_verts.end()) {
    vIndex = static_cast<size_t>(pos - m_verts.begin());

  } else {
    // v didn't exist, so let's insert
    vIndex = m_verts.size();
    m_verts.push_back(v);
  }

  return vIndex;
}


void TriangleMesh::triangulateImplicitFunc(float xmin, float xmax,
					   float ymin, float ymax,
					   float zmin, float zmax,
					   float dx,
					   double (*fnPtr)(double, double, double)) {
  int iTriCount;
  float *afVertices = new float[(sizeof(float) * 100)]; // shouldn't need more than this per cell

  float x, y, z;
  for (x = xmin; x < xmax; x += dx) {
    for (y = ymin; y < ymax; y += dx) {
      for (z = zmin; z < zmax; z += dx) {
	vMarchCubeCustom(x, y, z, dx, 
			 fnPtr,
			 iTriCount,
			 afVertices);

	for (int t = 0; t < iTriCount; t++) {
	  Vertex v1, v2, v3;

	  // 3 verts per triangle, 3 coords per vert:

	  v1.x[0] = roundDouble (afVertices[t*9]);
	  v1.x[1] = roundDouble (afVertices[t*9 + 1]);
	  v1.x[2] = roundDouble (afVertices[t*9 + 2]);

	  v2.x[0] = roundDouble (afVertices[t*9 + 3]);
	  v2.x[1] = roundDouble (afVertices[t*9 + 4]);
	  v2.x[2] = roundDouble (afVertices[t*9 + 5]);

	  v3.x[0] = roundDouble (afVertices[t*9 + 6]);
	  v3.x[1] = roundDouble (afVertices[t*9 + 7]);
	  v3.x[2] = roundDouble (afVertices[t*9 + 8]);
	    
	  addTriangle(v1, v2, v3);
	}
	
      }
    }
  }  
}


void TriangleMesh::print() const {
  std::cout << m_verts.size() << " vertices, " << m_faces.size() << " faces." << std::endl;
}


void TriangleMesh::write(const std::string &filename, MeshFileFormat mff) const {
  if (mff == MFF_OBJ) {
    writeObj(filename);
  } else {
    std::cerr << "Mesh file format not implemented. Exiting..." << std::endl;
    exit(1);
  }
}


void TriangleMesh::writeObj(const std::string &filename) const {

  if (m_verts.size() == 0) {
    std::cout << "!!!!!   Mesh has no vertices  !!!!!!" << std::endl;
    return;
  }

  std::ofstream ofile;
  ofile.open(filename.c_str());

  for (size_t i = 0; i < m_verts.size(); i++) {
    ofile << "v " 
	  << std::setprecision(6) 
	  << std::fixed 
	  << m_verts[i].x[0] << " "
	  << m_verts[i].x[1] << " "
	  << m_verts[i].x[2] << "\n";
  }

  for (size_t i = 0; i < m_faces.size(); i++) {
    // remember, vertex indices START FROM 1 (in OBJ format)!!!
    ofile << "f "
	  << m_faces[i]->v[0] + 1 << " "
	  << m_faces[i]->v[1] + 1 << " "
	  << m_faces[i]->v[2] + 1 << "\n";
  }

  ofile.close();
}


void TriangleMesh::read(const std::string &filename, MeshFileFormat mff) {
  if (mff == MFF_OBJ) {
    readObj(filename);
  } else {
    std::cerr << "Unimplemented mesh file format. Exiting..." << std::endl;
    exit(1);
  }
}

void TriangleMesh::readObj(const std::string &filename) {
  std::ifstream ifile(filename.c_str());
  std::string line;

  if (ifile.is_open()) {
    while (getline (ifile,line)) {
      std::cout << line << std::endl;
      }

    ifile.close();

  } else {
    std::cout << "Unable to open file" << std::endl; 
  }

}




// source: http://stackoverflow.com/questions/1406029/how-to-calculate-the-volume-of-a-3d-mesh-object-the-surface-of-which-is-made-up
double TriangleMesh::signedVolumeOfTriangle(const Vertex &v1, 
					    const Vertex &v2, 
					    const Vertex &v3) const {
  double v321 = v3.x[0] * v2.x[1] * v1.x[2];
  double v231 = v2.x[0] * v3.x[1] * v1.x[2];
  double v312 = v3.x[0] * v1.x[1] * v2.x[2];
  double v132 = v1.x[0] * v3.x[1] * v2.x[2];
  double v213 = v2.x[0] * v1.x[1] * v3.x[2];
  double v123 = v1.x[0] * v2.x[1] * v3.x[2];
  return (1.0/6.0) * (-v321 + v231 + v312 - v132 - v213 + v123);
}


double TriangleMesh::volume() const {
  double vol = 0.0;

  for (size_t i = 0; i < m_faces.size(); i++) {
    Face *f = m_faces[i];
    Vertex v1 = m_verts[f->v[0]];
    Vertex v2 = m_verts[f->v[1]];
    Vertex v3 = m_verts[f->v[2]];
    vol += signedVolumeOfTriangle(v1, v2, v3);
  }

  return fabs(vol);
}


Vertex &TriangleMesh::getVertex(size_t faceIndex, size_t vertIndex) {
  if (vertIndex == 0) 
    return m_verts[m_faces[faceIndex]->v[0]];
  else if (vertIndex == 1)
    return m_verts[m_faces[faceIndex]->v[1]];
  else
    return m_verts[m_faces[faceIndex]->v[2]];
}



void TriangleMesh::colorNeighbors(size_t vertIndex, size_t color) {

  Vertex &currVert = m_verts[vertIndex];
  if (currVert.color) return;

  currVert.color = color;


  for (size_t i = 0; i < currVert.faces.size(); i++) {

    Face *currFace = currVert.faces[i]; 

    if (!currFace->color) {
      // color this face
      currFace->color = color;
    } 

    // recursive call to vertices      
    if (!m_verts[currFace->v[0]].color)
      colorNeighbors(currFace->v[0], color);

    if (!m_verts[currFace->v[1]].color)
      colorNeighbors(currFace->v[1], color);

    if (!m_verts[currFace->v[2]].color)
      colorNeighbors(currFace->v[2], color);

  }
}



void TriangleMesh::color() {

  _numColors = 0;

  bool uncoloredVertsExist = true;
  size_t firstUncoloredVert = 0;

  while (uncoloredVertsExist) {
    
    _numColors++;

    
    colorNeighbors(firstUncoloredVert, _numColors);

    // We have colored one mesh. Now, let's see if there's still 
    // work to do:
    firstUncoloredVert = m_verts.size();
    
    for (size_t i = 0; i < m_verts.size(); i++) {
      Vertex &currVert = m_verts[i];
      if (!currVert.color) {
	firstUncoloredVert = i;
	break;
      }
    }

    if (firstUncoloredVert == m_verts.size()) {
      uncoloredVertsExist = false;
    }

    std::cout << "first uncolored vert is now: " << firstUncoloredVert << std::endl;
  }
  
  std::cout << "Number of meshes: " << _numColors << std::endl;

  // DEBUG   ***  THERE SHOULD BE NO UNCOLORED VERTS
  for (size_t i = 0; i < m_verts.size(); i++) {
    if (!m_verts[i].color) {
      std::cout << "Vertex " << i << " is uncolored. Exiting..." << std::endl;
      assert(m_verts[i].color);
    }
  }

  for (size_t i = 0; i < m_faces.size(); i++) {
    if (!m_faces[i]->color) {
      std::cout << "Face " << i << " is uncolored. Exiting..." << std::endl;
      assert(m_faces[i]->color);
    }
  }

}


std::vector<TriangleMesh> *TriangleMesh::splitMeshes() {

  std::vector<TriangleMesh> *meshes = new std::vector<TriangleMesh>;

  for (size_t c = 0; c < _numColors; c++) {
    meshes->push_back(TriangleMesh());
  }
  
  for (size_t f = 0; f < m_faces.size(); f++) {
    Face *currFace = m_faces[f];
    size_t c = currFace->color - 1;
    //    std::cout << "Num meshes: " << meshes->size() << "    mesh num: " << c << "    color: " << currFace->color << std::endl;
    meshes->at(c).addTriangle(getVertex(f, 0),
			      getVertex(f, 1),
			      getVertex(f, 2));
	
    
	
  } 

  for (size_t m = 0; m < meshes->size(); m++) {
    std::cout << "**** MESH " << m << " has volume: " << meshes->at(m).volume() << std::endl;
  }

  return meshes;
}
