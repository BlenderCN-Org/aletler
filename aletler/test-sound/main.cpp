//
//  main.cpp
//  test-sound
//
//  Created by Phaedon Sinis on 1/2/14.
//  Copyright (c) 2014 Phaedon Sinis. All rights reserved.
//

#include <iostream>

#include <sound/BoundaryPressure.h>
#include <geometry/TriangleMesh.h>

// Let's make sure I'm parsing the boundary pressure outputs from bem++ correctly
void bpressurefiletester() {
  TriangleMesh combined;
  
  combined.read("/Users/phaedon/github/aletler/meshes/geometrySim/air_000000.obj", MFF_OBJ);
  combined.read("/Users/phaedon/github/aletler/meshes/geometrySim/solid_000000.obj", MFF_OBJ);

  
  std::string filename = "/Users/phaedon/fakebemout.dat";
  
  BoundaryPressure bp;
  bp.read(filename);
  bp.visualize(combined);
}



int main(int argc, const char * argv[])
{

  bpressurefiletester();
  return 0;
}

