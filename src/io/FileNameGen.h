//
//  FileNameGen.h
//  aletler
//
//  Created by Phaedon Sinis on 1/7/14.
//  Copyright (c) 2014 Phaedon Sinis. All rights reserved.
//

#ifndef aletler_FileNameGen_h
#define aletler_FileNameGen_h

#include <iomanip>
#include <sstream>

// Helper function from StackOverflow
static std::string ZeroPadNumber(size_t num, int width)
{
  std::ostringstream ss;
  ss << std::setw( width ) << std::setfill( '0' ) << num;
  return ss.str();
}

static std::string fileNameSuffix(size_t bubbleNum, size_t frameNum) {
  return "_" + ZeroPadNumber(bubbleNum, 6) + "_" + ZeroPadNumber(frameNum, 6);
}

static std::string fastBEMFilename(const std::string &baseDir,
                                   const std::string &subDir,
                                   size_t bubbleNum, size_t frameNum) {
  return baseDir + "/" + subDir + "/"
  + "fastbem" + fileNameSuffix(bubbleNum, frameNum) + ".dat";
}

static std::string bubbleFreqFilename(const std::string &baseDir,
                                      const std::string &subDir,
                                      size_t bubbleNum) {
  return baseDir + "/" + subDir + "/"
  + "bubblefreq_" + ZeroPadNumber(bubbleNum, 6) + ".txt";

}

static std::string bubbleMeshFilename(const std::string &baseDir,
                                      const std::string &subDir,
                                      size_t bubbleNum, size_t frameNum) {
  return baseDir + "/" + subDir + "/"
  + "bubble" + fileNameSuffix(bubbleNum, frameNum) + ".obj";
}

static std::string airMeshFilename(const std::string &baseDir,
                                      const std::string &subDir,
                                      size_t frameNum) {
  return baseDir + "/" + subDir + "/"
  + "air" + "_" + ZeroPadNumber(frameNum, 6)  + ".obj";
}

static std::string solidMeshFilename(const std::string &baseDir,
                                   const std::string &subDir,
                                   size_t frameNum) {
  return baseDir + "/" + subDir + "/"
  + "solid" + "_" + ZeroPadNumber(frameNum, 6)  + ".obj";
}

static std::string velocityFilename(const std::string &baseDir,
                                    const std::string &subDir,
                                    size_t bubbleNum,
                                    size_t frameNum) {
  return baseDir + "/" + subDir + "/"
  + "airvel" + fileNameSuffix(bubbleNum, frameNum) + ".txt";
}


#endif
