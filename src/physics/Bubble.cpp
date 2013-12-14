//
//  Bubble.cpp
//  aletler
//
//  Created by Phaedon Sinis on 11/29/13.
//  Copyright (c) 2013 Phaedon Sinis. All rights reserved.
//

#include "Bubble.h"

#include <iomanip>
#include <sstream>


// Helper function from StackOverflow -- this should be moved
// out of this class
std::string ZeroPadNumber(size_t num, int width)
{
  std::ostringstream ss;
  ss << std::setw( width ) << std::setfill( '0' ) << num;
  return ss.str();
}

void Bubble::io_loadMeshes(const std::string &surface_name,
                   const std::string &bubble_name,
                   size_t bubble_index,
                   size_t frame_index,
                   int num_digits_bubble,
                   int num_digits_frame) {
  
  _bubble_index = bubble_index;
  
  // goal: given "surface", "b", [bubble_index] 12, [frame_index] 24, [bubble] 6, [frame] 6,
  // assemble this into <directory>/surface_000024.obj and <directory>/b000012_000024.obj
  // and load into the private members _surface and _bubble.
  
  std::string surfacepath = _directory;
  //surfacepath.append("/");
  surfacepath.append(surface_name);
  //surfacepath.append(surface_name + '_');
  //surfacepath.append(ZeroPadNumber(frame_index, num_digits_frame));
  surfacepath.append(".obj");
  
  std::cout << "Surface path:  " << surfacepath << std::endl;
  
  _surface->read(surfacepath, MFF_OBJ);
  
  std::string bubblepath = _directory;
  //bubblepath.append("/");
  bubblepath.append(bubble_name);
  //bubblepath.append(ZeroPadNumber(bubble_index, num_digits_bubble) + '_');
  bubblepath.append(ZeroPadNumber(frame_index, num_digits_frame));
  bubblepath.append(".obj");
  
  std::cout << "Bubble path:  " << bubblepath << std::endl;

  
  _bubble->read(bubblepath, MFF_OBJ);
  
  std::string solidpath = _directory;
  solidpath.append("solid_glass.obj");
  _solid->read(solidpath, MFF_OBJ);
  
}