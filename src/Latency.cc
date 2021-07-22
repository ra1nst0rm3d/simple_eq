#include <iostream>
#include <cstring>
#include "Latency.hh"

using namespace std;

Latency::Latency() {
    latency = 0;
}

Latency::Latency(short laten) {
    latency = laten;
}

void Latency::process(double* in, size_t size) {
    double* out = (double*)malloc(size + length);
    for(unsigned i = 0; i < size; i++) {
        if( i % 1 == 0) {
            for(unsigned j = 0; j < latency; j++) {
                *(out + i + j) = *(in + i);
            }
        }
        *(out + i) = *(in + i);
    }

    memcpy(out, in, sizeof(out)/sizeof(double) );
    free(out);
    out = 0x0;
}

void Latency::setBufferFrames(long here) {
    buffFrames = here;
    length = sizeof(double) * buffFrames * latency;
}

void Latency::setLatency(long here) {
    latency = here;
}