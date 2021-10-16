/* Copyright (C) Oleg Sazonov */

#pragma once
#include <cstdint>
#include <vector>
using namespace std;

typedef enum FilterType {
    LOW_SHELF = 1,
    HIGH_SHELF = 0,
    PEAK = 2,
    ALL_PASS = 3
} FilterType;

class Filter {

    private:
    double a1,a2,b0,b1,b2;

    double Q = 2;

    unsigned GainFreq, sampleRate;
    double gain;
    FilterType filt;
    //double x1 = 0, x2 = 0, y1 = 0, y2 = 0;
    double s1 = 0, s2 = 0;
    int reCalculate();

    public:
    Filter(int freq = 50, double gain = 3.5, enum FilterType filt = PEAK, unsigned sampleRate = 44100, double Q = 2);
    double process(double in);
    void clear();

    void setGainFreq(unsigned freq);
    void setGain(double gain);
    int setFilterType(FilterType filt);
    void setSampleRate(unsigned sampleRate);
    void setQValue(double Q);
};

static vector<Filter> filters;
