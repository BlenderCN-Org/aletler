//
//  ZeroCrossing.h
//  soundmath
//
//  Created by Phaedon Sinis on 4/16/13.
//  Copyright (c) 2013 Phaedon Sinis. All rights reserved.
//

#ifndef soundmath_ZeroCrossing_h
#define soundmath_ZeroCrossing_h

#include <fstream>

class ZeroCrossing {

public:
    ZeroCrossing(const SoundTrack *st) : m_soundtrack(st) {
    }
    
    void analyze() {
        size_t nsamp = m_soundtrack->numSamples();
        
        int last_zerox = 0;
        
        // j is used to look ahead to the next zero crossing
        int i = 0, j = 1;
        
        while (i < nsamp) {
            
            //std::cout << "i:" << i << "   j:" << j << std::endl;
            
            // Look ahead
            
            // Get through all the negative zones
            while ((j < nsamp) && m_soundtrack->sample(j, 0) < 0) {
                j++;
            }
            
            // Now get through the entire positive zone:
            while (j < nsamp && m_soundtrack->sample(j, 0) >= 0) {
                j++;
            }
            
            // reached the end? get out
            if (j >= nsamp)
                return;
            
            // Now, there should be a zero crossing from j-1 (+) to j (-) :
            assert(m_soundtrack->sample(j-1, 0) >= 0 && m_soundtrack->sample(j, 0) < 0);
            
            // Let's interp to get the exact zero crossing position:
          
            double slope = m_soundtrack->sample(j, 0) - m_soundtrack->sample(j-1, 0);
            double diff = -m_soundtrack->sample(j-1, 0) / slope;
            double next_zerox = (j-1) + diff;
            
            
            // avoid divide-by-zero:
            assert(next_zerox != last_zerox);
            
            double inst_freq = m_soundtrack->sampleRate() / (next_zerox - last_zerox);
            //std::cout << "inst freq: " << inst_freq  << std::endl;
            
            while (i < j) {
                
                // to avoid the erroneous spike in the beginning, we ignore
                // the first reading:
                if (last_zerox != 0) {
                    m_freqs.push_back(inst_freq);
                }
                i++;
            }
            j++;
            last_zerox = next_zerox;
            
            
        }
    }
    
    
    void outputToText() {
        std::ofstream freqdata;
        freqdata.open("/Users/phaedon/freqdata.txt");
        for (int i = 0; i < m_freqs.size(); i+=10) {
            freqdata << m_freqs[i] << std::endl;
        }
        freqdata.close();
    }
    
    
private:
    const SoundTrack *m_soundtrack;
    std::vector<double> m_freqs;
    
};

#endif
