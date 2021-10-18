/* Copyright (C) 2021 Oleg Sazonov */
#include <rtaudio/RtAudio.h>
#include <iostream>
#include <cstring>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <future>
#include <thread>

#include "Filter.hh"
#include "Latency.hh"
#include "Config.hh"

#include <signal.h>


using namespace std;

static RtAudio aud;
static RtAudio::StreamParameters iPar, oPar;
static double gainDB; // for signal gaining

// Pass-through function.
int inout( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
           double streamTime, RtAudioStreamStatus status, void *data )
{

  // Since the number of input and output channels is equal, we can do
  // a simple buffer copy operation here.
  if ( status ) std::cout << "Stream over/underflow detected." << std::endl;
   //chrono::steady_clock::time_point begin = chrono::steady_clock::now();
  for(vector<Filter>::iterator it = filters.begin(); it < filters.end(); it++) {
    it->process((double*)outputBuffer, (double*) inputBuffer, nBufferFrames * CHANNELS);
  }
    //chrono::steady_clock::time_point end = chrono::steady_clock::now();
  //cout << "Latency: " << chrono::duration_cast<chrono::nanoseconds>(end - begin).count() << "ns\n";
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
        if(line.find("//") != std::string::npos ) { continue; }
        else if (line.find("gain") != std::string::npos) {
            sscanf(line.c_str(), "%s %lf", (char*)NULL, &gainDB);
        } else if(!line.empty()) {
        int freq,filter_type;
        double Q,gain;
        sscanf(line.c_str(), "%d %lf %lf %d", &freq, &gain, &Q, &filter_type);
//      printf("%d %lf %lf %d\n", freq, gain, Q, filter_type);
        Filter f(freq, gain, (FilterType) filter_type, Q);
        filters.push_back(f);
        } else {continue;}
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
    RtAudio::StreamOptions opt;
    RtAudio::DeviceInfo info = aud.getDeviceInfo(oPar.deviceId);
    opt.flags = RTAUDIO_MINIMIZE_LATENCY;
    //cout << *(info.sampleRates.end() - 1) << endl; // finding max possible sampleRate
    
    for(vector<Filter>::iterator it = filters.begin(); it < filters.end(); it++) {
        it->setSampleRate(*(info.sampleRates.end() - 1));
    }
    

    try {
	    unsigned frames = BUFFER_FRAMES;
        aud.openStream(&oPar, &iPar, SAMPLE_TYPE, *(info.sampleRates.end() - 1), &frames, &inout, &opt);
    }
    catch (RtAudioError& e) {
        e.printMessage();
        return 0;
    }

    if(signal(SIGINT, signal_handle) == SIG_ERR) {
        cout << "Failed to set signal!" << endl;
        aud.stopStream();
        aud.closeStream();
        return -1;
    }

    auto as = async(launch::async, []{ audioProcess(); }); // just launch audio processing in another thread
    as.wait();

    aud.stopStream();
    if(aud.isStreamOpen()) aud.closeStream();
    return 0;
}
