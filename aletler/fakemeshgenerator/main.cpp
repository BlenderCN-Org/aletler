//
//  main.cpp
//  fakemeshgenerator
//
//  Created by Phaedon Sinis on 12/16/13.
//  Copyright (c) 2013 Phaedon Sinis. All rights reserved.
//

#include <iostream>

#include <geometry/TriangleMesh.h>
#include <sound/util.h>
#include <physics/Bubble.h>

static TriangleMesh *airMesh = NULL;
static TriangleMesh *solidMesh = NULL;

static const std::string baseDir = "/Users/phaedon/github/aletler/meshes/";

struct FakeBubbleStats {
  // set once
  double bubbleBirthtime;
  double bubbleRadius;
  
  // check at each frame;
  bool isBubbleBorn, isBubbleDead;
};

void simulateGeometry(size_t frameRate,
                      double minRadius, // in mm
                      double maxRadius, // in mm
                      double simDuration, // in seconds
                      size_t numBubbles, // over lifetime of simulation
                      const std::string &outputDir) {
 
  std::vector<FakeBubbleStats> bubbleStats(numBubbles);
  std::vector<Bubble *> bubbles(numBubbles);
  
  size_t numFrames = ceil(frameRate * simDuration);
  double dt = 1.0 / double(frameRate);
  
  // initialize random bubble stats
  for (size_t i = 0; i < numBubbles; i++) {
    bubbleStats[i].bubbleBirthtime = random_double(0.02, simDuration * 0.95);
    bubbleStats[i].bubbleRadius = random_double(minRadius, maxRadius);
    std::cout << "bubbleRadius: " << bubbleStats[i].bubbleRadius << std::endl;
    
    bubbleStats[i].isBubbleBorn = false;
    bubbleStats[i].isBubbleDead = false;
  }
  
  // initialize bubble objects
  for (size_t i = 0; i < numBubbles; i++) {
    bubbles[i] = new Bubble();
    TriangleMesh *currBubble = new TriangleMesh;
    
    // yep, for now a different copy of each
    currBubble->read(baseDir + "bubble_lr.obj", MFF_OBJ);
    
    // constant is here because starting size of bubbles is 5mm radius
    double ds = bubbleStats[i].bubbleRadius / 5.0;
    
    currBubble->scale(Vector3d(ds, ds, ds));
    
    double dx = random_double(-0.05, +0.05);
    double dz = random_double(-0.05, +0.05);
    Vector3d dpos(dx,0,dz);
    currBubble->translate(dpos);
    
    bubbles[i]->setBubbleMesh(currBubble);
    
  }
  
  std::string solidPrefix = "solid_";
  std::string airPrefix = "air_";
  std::string bubblePrefix = "bubble_";
  
  for (size_t f = 0; f < numFrames; f++) {
    
    double currTime = f * dt;
    
    std::string zeropadFrameNum = ZeroPadNumber(f, 6);

    // add a bit of random vertical motion to the fluid surface
    airMesh->jitter(Vector3d(0,0.0005,0));
    
    solidMesh->write(outputDir + solidPrefix + zeropadFrameNum + ".obj", MFF_OBJ);
    airMesh->write(outputDir + airPrefix + zeropadFrameNum + ".obj", MFF_OBJ);
    
    // advect all the bubbles for each timestep
    for (size_t b = 0; b < numBubbles; b++) {
      
      if (bubbleStats[b].bubbleBirthtime <= currTime) {
        bubbleStats[b].isBubbleBorn = true;
      }
      
      
      // check whether bubble has peeked above the surface
      if (bubbleStats[b].isBubbleBorn && !bubbleStats[b].isBubbleDead) {
        BoundingBox airbbox, bubbbox;
        airMesh->getBoundingBox(airbbox);
        bubbles[b]->getBubbleMesh()->getBoundingBox(bubbbox);
        
        // rough crappy rule of thumb
        if (bubbbox.GetBoxmax().y() > airbbox.GetBoxmax().y()) {
          // KILL THE BUBBLE!
          bubbleStats[b].isBubbleDead = true;
        }
      }
      
      
      if (bubbleStats[b].isBubbleBorn && !bubbleStats[b].isBubbleDead) {
        
        std::string zeropadBubbleNum = ZeroPadNumber(b, 6);
        bubbles[b]->getBubbleMesh()->write(outputDir + bubblePrefix + zeropadBubbleNum + "_" + zeropadFrameNum + ".obj", MFF_OBJ);
        bubbles[b]->timestep(dt);
      }
      
      
    }
    
    
    
  }
}
                      
void loadStartingMeshes() {
  
  airMesh = new TriangleMesh;
  airMesh->read(baseDir + "free_surface_glass.obj", MFF_OBJ);
  
  solidMesh = new TriangleMesh;
  solidMesh->read(baseDir + "solid_glass.obj", MFF_OBJ);
}


int main(int argc, const char * argv[]) {

  size_t framerate = 200;
  size_t numbubbles = 500;
  
  double smallestBub = 0.25;
  double biggestBub = 5.0;
  
  double simDuration = 10.0;
  
  loadStartingMeshes();
  
  simulateGeometry(framerate,
                   smallestBub, biggestBub,
                   simDuration,
                   numbubbles,
                   baseDir + "geomsim2/");
  
  
  return 0;
}

