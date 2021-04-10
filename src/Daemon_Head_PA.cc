/* Copyright (C) 2021 Oleg Sazonov */
#define DEFAULT_NAME "eq.cfg"
#define VERSION "0.1 (stable)"
#define SAMPLE_RATE 48000
#define SAMPLE_TYPE paFloat32

#include <portaudio.h>
#include "Filter.hh"
#include <iostream>
#include <cstring>
#include <fstream>
#include <vector>
#include <string>

using namespace std;


// Pass-through function.
int inout( const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData) 
{
    for(vector<Filter>::iterator it = filters.begin(); it < filters.end(); it++) {
        for(unsigned i = 0; i < 2 * framesPerBuffer; ++i) {
            *((double*)outputBuffer + i) = it->process(*((double*)inputBuffer + i));
            it->clear();
        }
    }

    return 0;
}

void update_coeffs(string name) {
    ifstream in;
    if(name.empty()) {
    in.open(DEFAULT_NAME);
    } else {
    in.open(name);
    }
    string line;
    filters.clear();

    if(!in.is_open()) {
        cout << "File not opened!" << endl;
        exit(-1);
    }
    while(getline(in, line)) {
        if(line.starts_with("//")) continue;
        int freq,gain,filter_type;
        double Q;
        sscanf(line.c_str(), "%d %d %lf %d", &freq, &gain, &Q, &filter_type);
        printf("%d %d %lf %d\n", freq, gain, Q, filter_type);
        Filter f(freq, gain, (enum FilterType) filter_type, 48000, Q);
        filters.push_back(f);
    }
}


int main(int argc, char* argv[]) {

    if(argv[0] == "-h"s) {
        cout << "ra1nst0rm3d's SimpleEQ version " << VERSION << endl << "U need to create a config file named " << DEFAULT_NAME << " or push name of file as first argument" << endl;
        return 0;
    } else if(argc > 1){
    update_coeffs(argv[1]);
    } else {
        update_coeffs(string(""));
    };

    if(Pa_Initialize() != paNoError) {
        cerr << "[Pa] No Init!" << endl;
        return -1;
    }

    PaStream *in;
    if(Pa_OpenDefaultStream( &in, 2, 2, SAMPLE_TYPE, SAMPLE_RATE, paFramesPerBufferUnspecified, inout, NULL) != paNoError) {
        Pa_Terminate();
        return -1;
    }
    
    Pa_StartStream(in);

    Pa_Sleep(15 * 1000);

    Pa_StopStream(in);

    Pa_CloseStream(in);
    

    Pa_Terminate();
    return 0;
}
