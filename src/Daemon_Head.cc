/* Copyright (C) 2021 Oleg Sazonov */
#include <rtaudio/RtAudio.h>
#include <iostream>
#include <cstring>
#include <fstream>
#include <vector>
#include <string>

#include "Filter.hh"
#include "Config.hh"

#include <signal.h>


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
      for(unsigned i = 0; i < 2 * nBufferFrames; i++) {
          *(d + i) = it->process(*(d + i));
      }
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
        if(line.find("//") != std::string::npos ) continue;
        int freq,gain,filter_type;
        double Q;
        sscanf(line.c_str(), "%d %d %lf %d", &freq, &gain, &Q, &filter_type);
        printf("%d %d %lf %d\n", freq, gain, Q, filter_type);
        Filter f(freq, gain, (enum FilterType) filter_type, 48000, Q);
        filters.push_back(f);
    }
}

void sig_handler(int signo) {
    if (signo == SIGINT) {
        cerr << "Freeing..." << endl;
        return;
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

    data.iPar.deviceId = 0;
    data.iPar.nChannels = 2;
    data.oPar.deviceId = 0;
    data.oPar.nChannels = 2;

    try {
        data.aud.openStream(&data.oPar, &data.iPar, SAMPLE_TYPE, SAMPLE_RATE, &BUFFER_FRAMES, &inout, NULL);
    }
    catch (RtAudioError& e) {
        e.printMessage();
        return 0;
    }

    try {
        data.aud.startStream();
reload:
        getchar();
        goto reload;
    }
    catch (RtAudioError &e) {
        e.printMessage();
    }
    data.aud.stopStream();
    if(data.aud.isStreamOpen()) data.aud.closeStream();
    return 0;
}
