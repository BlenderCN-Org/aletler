//
//  main.cpp
//  soundmath
//
//  Created by Phaedon Sinis on 11/30/12.
//  Copyright (c) 2012 Phaedon Sinis. All rights reserved.
//

#include <iostream>
#include <sound/SphericalTools.h>

#include <sound/SoundParameters.h>
#include <sound/Timer.h>
#include <sound/SoundTrack.h>
#include <sound/SoundFileManager.h>
#include <sound/Monopole.h>
#include <sound/Listener.h>
#include <sound/Bubble.h>
#include <sound/util.h>
#include <sound/ZeroCrossing.h>

void filter_simplest_lowpass(const vector<double> &x, vector<double> &y, void *args) {
    for (int i = 1; i < x.size(); i++) {
        y.push_back(x[i] + x[i-1]);
    }
}


// This just makes sure that our implementation of
// S_0_0 is correct, by verifying equality (2.1.107) on pg 65
// of Gumerov/Duraiswami
void test_sphericalbasis() {
    
    
    for (int i = 0; i < 1000; i++) {
        
        double k = (rand() % 20000) + 1;
        
        double x = (rand() % 200) - 100 + 0.5;
        double y = (rand() % 200) - 100 + 0.5;
        double z = (rand() % 200) - 100 + 0.5;

        Vector3d r(x, y, z);
        
        complex<double> s_0_0 = SphericalTools::S(0, 0, k, r);
        complex<double> g = SphericalTools::Greens(k, r);
        
        complex<double> g_s = ii * k * sqrt(0.25 * M_1_PI) * s_0_0;
        
        
        assert(fabs(g_s.real() - g.real()) < VERY_SMALL
               && fabs(g_s.imag() - g.imag() < VERY_SMALL) );
    }
}

void make_bubbles(std::vector<Bubble *> &bubblevec, int nBubbles,
                  int mm_smallest, int mm_largest,
                  int sec_duration) {
    
    for (int i = 0; i < nBubbles; i++) {
        
        // let's pick a size:
        double mmsize = random_double((double) mm_smallest, (double) mm_largest);
        
        // let's pick a birthtime
        double btime = random_double(0.0, (double) sec_duration);
        
        // let's pick a location
        double x = random_double(-1.0, 1.0);
        double y = random_double(-1.0, 1.0);
        Vector3d loc(x, y, 0);
        bubblevec.push_back(new Bubble(mmsize, btime, loc));
    }
    
    std::sort(bubblevec.begin(), bubblevec.end(), Bubble::sort_by_birth);
}


int main(int argc, const char * argv[])
{

  /*
    SoundTrack fizzy(44100, 1);
    //SoundFileManager mineralwater("/Users/phaedon/mineralwater.wav", 48000);
    SoundFileManager mineralwater("/Users/phaedon/16_chirp.wav", 48000);

    SoundFileManager fizzyout("/Users/phaedon/fizzy.aiff", 48000, 1);
    
    mineralwater.open(ReadOnly);
    mineralwater.readAudio(fizzy);
    mineralwater.close();

    fizzy.normalize();
    cout << "Number of fizzy samples: " << fizzy.numSamples() << endl;
    
   // fizzyout.open(WriteOnly);
    //fizzyout.writeAudio(fizzy);
    //fizzyout.close();
    
    cout << "Wrote fizzy water" << endl;
    
    ZeroCrossing zc(&fizzy);
    zc.analyze();
    zc.outputToText();
    
    cout << "Analyzed fizzy water" << endl;
    
    return 0;
  */
    
    
    
    test_sphericalbasis();
    
    SoundTrack muzak;
    Timer simulation_timer;
    
    simulation_timer.start_timer("Bubble simulation");
    
    // Bubbles!
    std::vector<Bubble *> lotsa_bubbles;
    
    double total_duration = 5;

    // one bubble.
    //lotsa_bubbles.push_back(new Bubble(10, 0.5, Vector3d(1, 1, 0)));

    // many bubbles.
    make_bubbles(lotsa_bubbles, 100, 5, 12, total_duration);
    
   
    // and someone needs to be able to hear it
    
    Listener human;
    human.addLocation(Vector3d(1, 0, 0));
    human.addLocation(Vector3d(1, HEAD_WIDTH, 0));
    
    
    double timestep = 1.0 / SAMPLING_RATE;
    
    
    cout << "Launching simulation..." << endl;

    
    for (int i = 0; i < SAMPLING_RATE * total_duration; i++) {
        
        double t = i * timestep;
        
        double leftearpressure = 0.0;
        double rightearpressure = 0.0;
        
        // add up the sound pressure from all the bubbles...
        for (int b = 0; b < lotsa_bubbles.size(); b++) {
            Bubble* curr_bubble = lotsa_bubbles[b];
            
            bool isBubbleBorn = i >= curr_bubble->get_birthtime() * SAMPLING_RATE;
            bool isBubbleDead = i > (curr_bubble->get_birthtime() + Bubble::S_BUBBLELIFE) * SAMPLING_RATE;
            
            if(isBubbleBorn && !isBubbleDead) {
                leftearpressure += curr_bubble->pressure(t, human.leftEar());
                rightearpressure += curr_bubble->pressure(t, human.rightEar());
            }
        }
        
        // insert the totals into the sound samples
        muzak.addSample(leftearpressure, 0);
        muzak.addSample(rightearpressure, 1);
  
       // movingthing.step(timestep);
    }

    //muzak.applyFilter(filter_simplest_lowpass);
    
    muzak.normalize();

    simulation_timer.stop_timer();
    cout << "Finished simulation. Saving..." << endl;
    
    // Save the sounds to disk!
    string directoryPath;
    
    // check your identity
    if(DirectoryExists("/Users/phaedon"))
       directoryPath = "/Users/phaedon";
 
    else if(DirectoryExists("/Users/katherine"))
        directoryPath = "/Users/katherine/fluid_sound/synth";
    
    else {
        std::cerr<<"WARNING: neither KB or PS detected to be running this program; printing output files to ~/. Happy hunting for them!\n";
        directoryPath = "~/";
    }
    
    string filename = directoryPath + "/" + "bubbles.aiff";
    SoundFileManager sndfile(filename.c_str());
    
    sndfile.open(WriteOnly);
    sndfile.writeAudio(muzak);
    sndfile.close();
    
        
    cout << "Finished!\n" << endl;
    
    return 0;
}
