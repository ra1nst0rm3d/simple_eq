/* Copyright (C) Oleg Sazonov */

#pragma once
#include <cstdint>
#include <vector>
using namespace std;

enum FilterType {
    LOW_SHELF,
    HIGH_SHELF
};

static unsigned numOfFilts;
class Filter {

    private:
    float a0,a1,a2,b0,b1,b2;
    double Q = 0.25;
    double x1 = 0, x2 = 0, y1 = 0, y2 = 0;


    public:
    Filter(int GainFreq, int gain, enum FilterType filt, unsigned smpl, double Q);
    vector<double> samples;
    double process(double in);
    void clear();

};
static vector<Filter> filters;