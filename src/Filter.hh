/* Copyright (C) Oleg Sazonov */

#pragma once
#include <cstdint>
#include <vector>
using namespace std;

enum FilterType {
    LOW_SHELF,
    HIGH_SHELF
};

static int numOfFilts;
class Filter {

    private:
    unsigned sample_freq;
    float a0,a1,a2,b0,b1,b2;
    double delay1[2] = {0.0f}, delay2[2] = {0.0f};
    enum FilterType filt;
    double Q = 0.25;

    public:
    Filter(int GainFreq, int gain, enum FilterType filt, unsigned smpl, double Q);
    vector<double> samples;
    double* process(double* in, size_t size);

};
static vector<Filter> filters;