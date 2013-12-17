//
//  Electrostatics.cpp
//  aletler
//
//  Created by Phaedon Sinis on 11/29/13.
//  Copyright (c) 2013 Phaedon Sinis. All rights reserved.
//


#include <physics/Electrostatics.h>
#include <vdb.h>

using Eigen::Vector3d;


void Electrostatics::precomputeDomainMatrix() {
  
  // size of domain matrix
  size_t nd = _na + _ns;
  
  _D.resize(nd, nd);
  _D.setZero();
  
  
  // Compute the left block (the Dirichlet elements)
  for (size_t r = 0; r < nd; r++) {

    Triangle ti;
    Vector3d cent;
    
    // Grab the appropriate triangle for computing the centroid (collocation)
    if (r < _na) {
      ti = _air->triangle(r);
    } else {
      ti = _solid->triangle(r - _na);
    }
    cent = ti.centroid();

    for (size_t c = 0; c < _na; c++) {
      
      Triangle tj = _air->triangle(c);

      _D(r, c) = (0.25 * M_1_PI) * tj.potential( cent );
      
    }
  }
  
  
  // Compute the right block (the Neumann elements)
  for (size_t r = 0; r < nd; r++) {
    
    Triangle ti;
    Vector3d cent;
    if (r < _na) {
      ti = _air->triangle(r);
    } else {
      ti = _solid->triangle(r - _na);
    }
    cent = ti.centroid();
    
    
    for (size_t c = _na; c < nd; c++) {
      
      if (r == c) {
        // set all the diagonals to 1/2 to solve the interior formulation
        _D(r, c) = +0.5;
      } else {
        
        Triangle tj = _solid->triangle(c - _na);
        
        _D(r, c) = neumannMatrixElem(tj, cent);
        
      }
    }
  }
}


void Electrostatics::computeBubbleSubmatrices() {
  size_t n = _nb + _na + _ns;
  
  _Abb.resize(_nb, _nb);
  _B.resize(_nb, _na + _ns);
  _C.resize(_na + _ns, _nb);

  // Let's do the left column: Abb and C.
  for (size_t r = 0; r < n; r++) {
    Triangle ti = triangleAt(r);
    Vector3d cent = ti.centroid();
    
    for (size_t c = 0; c < _nb; c++){
      
      Triangle tj = triangleAt(c);
      double dirichletElem = (0.25 * M_1_PI) * tj.potential( cent );
      if (r < _nb) {
        _Abb(r, c) = dirichletElem;
      } else {
        _C(r - _nb, c) = dirichletElem;
      }
    }
  }
  
  
  // Now let's do the matrix block along the top row: _B
  for (size_t r = 0; r < _nb; r++) {
    
    Triangle ti = triangleAt(r);
    Vector3d cent = ti.centroid();
    
    for (size_t c = 0; c < _na + _ns; c++) {
      Triangle tj = triangleAt(c + _nb);
      
      if (c < _na) {
        _B(r, c) = (0.25 * M_1_PI) * tj.potential( cent );
      } else {
        _B(r, c) = neumannMatrixElem(tj, cent);
      }
      
    }
  }
  
}




double Electrostatics::bubbleCapacitance() {
  
  fmbsolver.solve(_rhs, _x);
  return _x.head(_bubble->size()).dot(_bubble->triangleAreas()) * 0.25 * M_1_PI;
}




void Electrostatics::visualize() {
  
  
  size_t n = _nb + _na + _ns;
  
  for (size_t i = 0; i < n; i++) {
    Triangle t = triangleAt(i);
    Vector3d c = t.centroid();
    if (i < _nb)
      vdb_color(0, 0.2, 0.5);
    else if (i < _nb + _na)
      vdb_color(0.8, 0, 0.8);
    else
      vdb_color(0.8, 0.8, 0.1);
    vdb_point(c.x(), c.y(), c.z());
  }
}





/********************************
 *    DEPRECATED HALL OF SHAME
 ********************************/

/*

void Electrostatics::computeDirichletMatrix() {
  
  if (_triangles.size() == 0 || _centroids.size() == 0) {
    precomputeTriangles();
  }
  
  size_t n = _bubble->size() + _free_surface->size() + _solid->size();
  size_t n_bub_fs = _bubble->size() + _free_surface->size();
  _dirichletMatrix.resize(n, n);
  _dirichletMatrix.setZero();
  
  for (size_t i = 0; i < n; i++) {
    
    // because the last column is zeroed out in the multiplication
    // Hp = Gt where G is the dirichlet matrix, we don't bother
    for (size_t j = 0; j < n_bub_fs; j++) {
      
      _dirichletMatrix(i, j) = (0.25 * M_1_PI) * _triangles[j].potential( _centroids[i] );
      
    }
  }
}


void Electrostatics::computeNeumannMatrix() {
  
  size_t n = _nb + _na + _ns;
  
  
  if (_triangles.size() == 0 || _centroids.size() == 0) {
    precomputeTriangles();
  }
  
  _neumannMatrix.resize(n, n);
  _neumannMatrix.setZero();
  
  for (size_t i = 0; i < n; i++) {
    for (size_t j = 0; j < n; j++) {
      
      // only set non-diagonal entries:
      if (i != j) {
        
        // and only for the H_B and H_S columns (H_F is multiplied by zeros in the special case of
        // the free surface, so we leave it out to avoid integrating each of those triangles)
        if (j < _nb || j >= (_nb + _na)) {
          
          _neumannMatrix(i, j) = neumannMatrixElem(_triangles[j], _centroids[i]);
        }
        
        
      } else {
        
        // set all the diagonals to 1/2 to solve the interior formulation
        _neumannMatrix(i, i) = +0.5;
      }
      
    }
  } // end for-loops
  
  computeRHS();
  
}




void Electrostatics::computeCombinedMatrix() {
  
  
  size_t n = _nb + _na + _ns;
  
  _combinedMatrix.resize(n, n);
  _combinedMatrix.block(0, 0, n, _nb + _na) = _dirichletMatrix.block(0, 0, n, _nb + _na);
  _combinedMatrix.block(0, _nb + _na, n, _ns) = _neumannMatrix.block(0, _nb + _na, n, _ns);
  
}
 
 */


