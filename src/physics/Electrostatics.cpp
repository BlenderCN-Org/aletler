//
//  Electrostatics.cpp
//  aletler
//
//  Created by Phaedon Sinis on 11/29/13.
//  Copyright (c) 2013 Phaedon Sinis. All rights reserved.
//


#include <physics/Electrostatics.h>

using Eigen::Vector3d;


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
  
  size_t nb = _bubble->size();
  size_t nf = _free_surface->size();
  size_t n = nb + nf + _solid->size();

  
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
        if (j < nb || j >= (nb + nf)) {
          
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
  
  size_t nb = _bubble->size();
  size_t nf = _free_surface->size();
  size_t ns = _solid->size();
  size_t n = nb+nf+ns;
  
  _combinedMatrix.resize(n, n);
  _combinedMatrix.block(0, 0, n, nb + nf) = _dirichletMatrix.block(0, 0, n, nb + nf);
  _combinedMatrix.block(0, nb + nf, n, ns) = _neumannMatrix.block(0, nb + nf, n, ns);
}



void Electrostatics::solveLinearSystem() {
  computeCombinedMatrix();
  _q = _combinedMatrix.fullPivLu().solve(_rhs);
}



double Electrostatics::bubbleCapacitance() {
  return _q.head(_bubble->size()).dot(_bubble->triangleAreas()) * 0.25 * M_1_PI;
}

void Electrostatics::precomputeTriangles() {
  _triangles.clear();
  _centroids.clear();
  
  size_t nb = _bubble->size();
  size_t nf = _free_surface->size();
  size_t ns = _solid->size();
  
  for (size_t i = 0; i < nb+nf+ns; i++) {
    _triangles.push_back(triangleAt(i));
    _centroids.push_back(triangleAt(i).centroid());
  }
}
