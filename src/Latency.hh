#pragma once

#include <iostream>
using namespace std;

class Latency {
    public:
    void setBufferFrames(long here);
    void setLatency(long here);
    void process(double* in);
    void setChannels(unsigned short here);
    Latency(short laten);
    Latency();
    
    private:
    unsigned short latency, buffFrames, chan;
    unsigned long length, size;

};

