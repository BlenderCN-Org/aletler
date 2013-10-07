//
//  SoundTrack.cpp
//  soundmath
//
//  Created by Phaedon Sinis on 11/30/12.
//  Copyright (c) 2012 Phaedon Sinis. All rights reserved.
//

#include "SoundTrack.h"

#include <vector>
#include <algorithm>
#include <cmath>


void scale_filter (const std::vector<double> &x, std::vector<double> &y, void *vsscalefactor) {
    
    double scalefactor = * (double *)vsscalefactor;
    
    for (int i = 0 ; i < x.size(); i++) {
        y[i] = x[i] * scalefactor;
    }
}

SoundTrack::SoundTrack(int sr, int nchannels) : m_sr(sr), m_nchannels(nchannels) {
    
    m_single_track = NULL;
    
    // Allocate memory for the array of audio channels
    m_tracks = new std::vector<double>[m_nchannels];
    
}


SoundTrack::~SoundTrack() {
    
    delete [] m_tracks;
    if (m_single_track) delete m_single_track;
}


void SoundTrack::normalize() {
    double maxval = 0.0;
    
    // Find max value per track...
    for (int i = 0; i < m_nchannels; i++) {
        for (int j = 0; j < m_tracks[i].size(); j++) {
            
            if (fabs(m_tracks[i][j]) > maxval) {
                maxval = fabs(m_tracks[i][j]);
            }
        }
    }


    // Prevent divide-by-zero errors:
    double inv_maxval = 0.0;
    if (maxval != 0.0) {
        inv_maxval = 1.0 / maxval;
    }
    
    
    // Normalize!
    applyFilter(&scale_filter, &inv_maxval, true);
}



void SoundTrack::applyFilter(filterfn filter, void *args, bool inPlace) {

    for (int i = 0; i < m_nchannels; i++) {
        
        if (inPlace) {
            filter(m_tracks[i], m_tracks[i], args);
        }
        
        else {
            std::vector<double> newtrack;
            filter(m_tracks[i], newtrack, args);
        
            m_tracks[i].resize(newtrack.size());
            std::copy(newtrack.begin(), newtrack.end(), m_tracks[i].begin());
        }
    }
}



std::vector<double> *SoundTrack::interleave() const {
    
    // Tracks haven't yet been interleaved? Let's do it.
    
    std::vector<double> *track = new std::vector<double>;
    
    
    // Assume all tracks contain same number of samples
    verify_track_lengths();
    
    
    for (int i = 0; i < m_tracks[0].size(); i++) {
        for (int j = 0; j < m_nchannels; j++) {
            track->push_back(m_tracks[j][i]);
        }
    }
    
    return track;
}
