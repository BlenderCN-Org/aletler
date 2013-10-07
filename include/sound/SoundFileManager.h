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
    

    void close() {
        sf_close(m_sndfile);
    }
    
    void writeAudio(SoundTrack &st) {
        const std::vector<double> *samples = st.get_single_track();
        std::cout << "Writing # samples: " << samples->size() << std::endl;
        sf_write_double(m_sndfile, &samples->at(0), samples->size());
        sf_write_sync(m_sndfile);
    }
    
    void readAudio(SoundTrack &st) {
        
        sf_count_t count;
        double *ptr = new double[m_sfinfo.frames];
        
        std::cout << "Num FRAMES: " << m_sfinfo.frames << std::endl;
        
        count = sf_read_double(m_sndfile, ptr, m_sfinfo.frames) ;
        
        std::cout << "Num COUNT: " << count << std::endl;

        
        for (int i = 0; i < count; i++) {
            st.addSample(ptr[i], 0);
        }
        
        
        
        delete ptr;
    }
    
    
    
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
