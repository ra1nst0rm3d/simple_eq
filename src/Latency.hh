#pragma once

#include <iostream>
using namespace std;

class Latency {
    public:
    void setBufferFrames(long here);
    void setLatency(long here);
    void process(double* in);
    Latency(short laten);
    Latency();
    
    private:
    short latency, buffFrames;
    long length, size;

};

