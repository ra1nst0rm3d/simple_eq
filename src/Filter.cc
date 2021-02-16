/* Copyright (C) 2020 Oleg Sazonov */
#include <iostream>
#include <cmath>
#include "Filter.hh"

Filter::Filter(int GainFreq, int gain, enum FilterType filt, unsigned smpl, double Q) {
    //const double A = sqrt(pow(10.0, (gain)/20.0));
    const double w = 2 * M_PI* GainFreq/smpl;
    //const double alpha = w / (w + 1.0);
    double ws=sin(w),wc=cos(w);
    double alpha = ws / (2 * Q);
    switch (filt) {
            case LOW_SHELF:
                b0 = (1.0 - wc) * 0.5;
                b1 = 1.0 - wc;
                b2 = (1.0 - wc) * 0.5;
                break;
            case HIGH_SHELF:
                b0 = (1.0 + wc) * 0.5;
                b1 = -(1.0 + wc);
                b2 = (1.0 + wc) * 0.5;
                break;
            default:
                cerr << "[FILT] No way! U just don't selected filter type!" << endl;
                exit(0);      
    }
    a0 = 1.0 + alpha;
    a1 = -2.0 * wc;
    a2 = 1.0 - alpha;
    this->filt = filt;
    this->sample_freq = smpl;
}

void Filter::clear() {
    x1 = x2 = y1 = y2 = 0;
}

double Filter::process(double in) {
    this->clear();
    double output;

    output = (b0 / a0) * in + (b1 / a0) * x1 + (b2 / a0) * x2 - (a1 / a0) * y1 - (a2 / a0) * y2;
    x2 = x1;
    x1 = in;
    y2 = y1;
    y1 = output;
    return output;
}
