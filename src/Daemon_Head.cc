/* Copyright (C) 2021 Oleg Sazonov */
#include <rtaudio/RtAudio.h>
#include <iostream>
#include <cstring>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <future>

#include "Filter.hh"
#include "Config.hh"

#include <signal.h>


using namespace std;

static RtAudio aud;
static RtAudio::StreamParameters iPar, oPar;


// Pass-through function.
int inout( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
           double streamTime, RtAudioStreamStatus status, void *data )
{

  // Since the number of input and output channels is equal, we can do
  // a simple buffer copy operation here.
  if ( status ) std::cout << "Stream over/underflow detected." << std::endl;

  for(vector<Filter>::iterator it = filters.begin(); it < filters.end(); it++) {
      for(unsigned i = 0; i < CHANNELS * nBufferFrames; i++) {
          *((double*)inputBuffer + i) = it->process(*((double*)inputBuffer + i));
      }
  }

  memcpy( outputBuffer, inputBuffer, nBufferFrames * CHANNELS * sizeof(double) );
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
        //printf("%d %d %lf %d\n", freq, gain, Q, filter_type);
        Filter f(freq, gain, (enum FilterType) filter_type, SAMPLE_RATE, Q);
        filters.push_back(f);
    }
}

void signal_handle(int sig) {
    if(sig == SIGINT) {
        cout << endl << "Shutdown..." << endl;
        aud.stopStream();
        aud.closeStream();
        exit(0);
    }
}

void audioProcess() {
    try {
        aud.startStream();
        for(;;) {this_thread::sleep_for(chrono::seconds(10));}
    }
    catch (RtAudioError &e) {
        e.printMessage();
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

    

    iPar.deviceId = 0;
    iPar.nChannels = CHANNELS;
    oPar.deviceId = 0;
    oPar.nChannels = CHANNELS;

    if(signal(SIGINT, signal_handle) == SIG_ERR) {
        cout << "Failed to set signal!" << endl;
        return -1;
    }

    try {
        aud.openStream(&oPar, &iPar, SAMPLE_TYPE, SAMPLE_RATE, &BUFFER_FRAMES, &inout, NULL);
    }
    catch (RtAudioError& e) {
        e.printMessage();
        return 0;
    }

    auto as = async(launch::async, []{ audioProcess(); }); // just launch audio processing in another thread
    as.wait();

    aud.stopStream();
    if(aud.isStreamOpen()) aud.closeStream();
    return 0;
}
