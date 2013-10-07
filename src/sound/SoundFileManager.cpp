//
//  SoundFileManager.cpp
//  soundmath
//
//  Created by Phaedon Sinis on 11/30/12.
//  Copyright (c) 2012 Phaedon Sinis. All rights reserved.
//

#include "SoundFileManager.h"
#include <iostream>

using namespace std;

SoundFileManager::SoundFileManager(const char *filename, int samplerate, int nchannels) {
    
    init_sfinfo(samplerate, nchannels);
    
    m_filename = filename;
    
    m_sndfile = NULL;
}



void SoundFileManager::open(SFMOpenMode openmode) {
    if (openmode == WriteOnly) {
        
        m_sndfile = sf_open(m_filename.c_str(), SFM_WRITE, &m_sfinfo);
        
    } else {
        
        // "When opening a file for read, the format field
        // should be set to zero before calling sf_open()."
        m_sfinfo.format = 0;
        
        m_sndfile = sf_open(m_filename.c_str(), SFM_READ, &m_sfinfo);
        
        std::cout << "Opened file. " << m_sfinfo.channels << " channels. " << m_sfinfo.samplerate << " samplerate." << std::endl;
    }

}


void SoundFileManager::init_sfinfo(int samplerate, int nchannels) {
    
    m_sfinfo.samplerate = samplerate;    
    m_sfinfo.channels = nchannels;

    // and this is standard:
    m_sfinfo.format = SF_FORMAT_AIFF | SF_FORMAT_PCM_16;

    if (! sf_format_check(& m_sfinfo)) {
        
        // assert something horrible and barf
        cout << "SF_INFO struct not properly initialized. Exiting..." << endl;
        exit(-1);
    }    
}
