//
//  SoundTrack.h
//  soundmath
//
//  Created by Phaedon Sinis on 11/30/12.
//

#ifndef soundmath_SoundTrack_h
#define soundmath_SoundTrack_h

#include <vector>
#include <cmath>
#include <cassert>

#include <iostream>

typedef void (* filterfn) (const std::vector<double> &x, std::vector<double> &y, void *args);


class SoundTrack {
    
public:
    SoundTrack(int sr = 44100, int nchannels = 2);
    ~SoundTrack();


    void normalize();
    

    void addSample(double samp, int chann) {
        assert(chann < m_nchannels);
        m_tracks[chann].push_back(samp);
    }
    
    
    //const std::vector<double> &get_track() const { return m_track; }
    
    double sample(int sampnum, int chann) const {
        return m_tracks[chann].at(sampnum);
    }
    
    std::vector<double> *get_single_track() {
        
        if (! m_single_track) {
            m_single_track = interleave();
        }
        
        assert(m_single_track);
        assert(m_single_track->size() > 0);

        for (int i = 0; i < m_single_track->size(); i++) {
            if (m_single_track->at(i) != 0.0) {
                std::cout << "Ok, they're not all zero. First nonzero value: " << m_single_track->at(i) << std::endl;
                break;
            }
        }
        return m_single_track;
    }
    
    size_t numSamples() const { return m_tracks[0].size(); }
    
    int sampleRate() const {return m_sr;}

    
    void applyFilter(filterfn filter, void *args = NULL, bool inPlace = false);
    
private:
    
    // We have to interleave the samples to have multiple channels
    // This is computed the first time the single track is requested.
    std::vector<double> *m_single_track;
    
    // Each channel has a separate track
    // This is where intermediate work is done (like filtering and
    // accumulation of samples)
    std::vector<double> *m_tracks;
    
    int m_nchannels;
    
    // Sample rate
    int m_sr;
    
    std::vector<double> *interleave() const;
    
    void verify_track_lengths() const {
        size_t numSamples = m_tracks[0].size();
        for (int i = 0; i < m_nchannels; i++) {
            assert(m_tracks[i].size() == numSamples);
        }
    }
};


#endif
