//
//  SoundFileManager.h
//  This is just a C++ wrapper around libsndfile, with some default settings,
//  to make it easy to write sound files.
//
//  Created by Phaedon Sinis on 11/30/12.
//

#ifndef soundmath_SoundFileManager_h
#define soundmath_SoundFileManager_h



// libsndfile is the bomb: http://www.mega-nerd.com/libsndfile/api.html
#include <sndfile.h>
#include <string>

#include "SoundTrack.h"


enum SFMOpenMode {ReadOnly, WriteOnly};

class SoundFileManager {
    
 public:
  SoundFileManager(const char *filename, int samplerate = 44100, int nchannels = 2);
    
  void open(SFMOpenMode openmode);
  void close();
    
  void writeAudio(SoundTrack &st);
  void readAudio(SoundTrack &st);    
    
    
 private:
  std::string m_filename;
    
  SF_INFO m_sfinfo;
  SNDFILE *m_sndfile;
    
    
  /**
   * Initializes a sound file info struct with some
   * default options.
   */
  void init_sfinfo(int samplerate, int nchannels);

    
};


#endif
