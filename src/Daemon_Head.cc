#include <rtaudio/RtAudio.h>
#include <libconfig.h++>
#include "Filter.hh"
#include <iostream>

using namespace std;
int inout( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
           double streamTime, RtAudioStreamStatus status, void *data )
{
  if ( status ) std::cout << "Stream over/underflow detected." << std::endl;
  double* data_S = (double*) inputBuffer;
  for(int i = 0; i < numOfFilts; i++) {
      data_S = filters.at(i).process(data_S, sizeof(data_S)/sizeof(double));
  }
  outputBuffer = data_S;
  return 0;
}


int main() {
    libconfig::Config cfg;
    RtAudio adac;
    try {
        cfg.readFile("eq.cfg");
    }
    catch(const libconfig::FileIOException &fioex) { cout << "[libconfig] I/O error" << endl;}
    catch(const libconfig::ParseException &pex) {cout << "[libconfig] Parse error at" << pex.getFile() << ":" << pex.getLine() << " - " << pex.getError() << endl;}

    const libconfig::Setting& root = cfg.getRoot();

    root.lookupValue("numOfFilts", numOfFilts);
    try {
        const libconfig::Setting& filters_s = root["filters"];
    }
    catch(const libconfig::ParseException &pex) {cout << "[libconfig] Parse error at" << pex.getFile() << ":" << pex.getLine() << " - " << pex.getError() << endl;}
    const libconfig::Setting& filters_s = root["filters"];
    float gain,Q,freq;
    int filter_type;
    for(unsigned i = 0; i < numOfFilts; i++) {
        const libconfig::Setting& filter = filters_s[i];

        if(!(filter.lookupValue("freq", freq)
        && filter.lookupValue("gain", gain)
        && filter.lookupValue("Q", Q)
        && filter.lookupValue("filter_type", filter_type))) {
            continue;
        } else {
            cerr << "[libconfig] Error at " << i << "filter, aborting!" << endl;
            return -1;
        }
        
        filters.at(i) = Filter()
    }
}