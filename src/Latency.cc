#include <iostream>
#include <cstring>
#include "Latency.hh"
#include "Config.hh"

using namespace std;

Latency::Latency() {
    latency = 0;
}

Latency::Latency(short laten) {
    latency = laten;
}

void Latency::process(double* in) {
    if(latency == 0) return;
//    cout << "Here: ptr = " << in << " , size = " << size << endl;
    double* out = (double*)malloc(size + length);
    for(unsigned i = 0; i < size; i++) {
        if( i % 1 == 0) {
            for(unsigned j = 0; j < latency; j++) {
                *(out + i + j) = *(in + i);
            }
        }
        *(out + i) = *(in + i);
    }

    memcpy(out, in, size + length );
    free(out);
    out = 0x0;
}

void Latency::setBufferFrames(long here) {
    buffFrames = here;
    length = sizeof(double) * buffFrames * latency;
    size = chan * buffFrames;
}

void Latency::setLatency(long here) {
    latency = here;
}

void Latency::setChannels(unsigned short here) {
    chan = here;
}