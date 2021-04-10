/* Copyright (C) 2021 Oleg Sazonov */
#define DEFAULT_NAME "eq.cfg"
#define VERSION "0.1 (stable)"

#include <rtaudio/RtAudio.h>
#include "Filter.hh"
#include <iostream>
#include <cstring>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

struct CustomData {
    RtAudio aud;
    RtAudio::StreamParameters iPar, oPar;
};

// Pass-through function.
int inout( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
           double streamTime, RtAudioStreamStatus status, void *data )
{
  // Since the number of input and output channels is equal, we can do
  // a simple buffer copy operation here.
  if ( status ) std::cout << "Stream over/underflow detected." << std::endl;
  unsigned long bytes = nBufferFrames * 2 * sizeof(double);
  double* d = (double*) inputBuffer;
  for(vector<Filter>::iterator it = filters.begin(); it < filters.end(); it++) {
          d = it->process(d, nBufferFrames * 2);
  }

  memcpy( outputBuffer, inputBuffer, bytes );
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

    struct CustomData data;

    unsigned int bufferFrames = 1024;
    data.iPar.deviceId = 0;
    data.iPar.nChannels = 2;
    data.oPar.deviceId = 0;
    data.oPar.nChannels = 2;

    try {
        data.aud.openStream(&data.oPar, &data.iPar, RTAUDIO_FLOAT64, 48000, &bufferFrames, &inout, NULL);
    }
    catch (RtAudioError& e) {
        e.printMessage();
        return 0;
    }

    try {
reload:
        data.aud.startStream();
        char c = getchar();
        if(c == 'u') {
            data.aud.stopStream();
            update_coeffs(string(""));
            goto reload;
        }

        data.aud.stopStream();
    }
    catch (RtAudioError &e) {
        e.printMessage();
        if(data.aud.isStreamOpen()) data.aud.closeStream();
        return 0;
    }
    if(data.aud.isStreamOpen()) data.aud.closeStream();
    return 0;
}
