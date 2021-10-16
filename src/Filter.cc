/* Copyright (C) 2020 Oleg Sazonov */
#include <iostream>
#include <cmath>
#include "Filter.hh"

Filter::Filter(int GainFreq, double gain, FilterType filt, unsigned smpl, double Q) {
   this->GainFreq = GainFreq;
   this->gain = gain;
   this->filt = filt;
   this->sampleRate = smpl;
   this->Q = Q;
   reCalculate(); 
}
void Filter::clear() {
    s1 = s2 = 0;
}
void Filter::process(double* output, double* input, size_t size) {
    unsigned i = 0;
    double *out = (double*)output, *in = (double*)input;

    while(i != size) {
    *out = s1 + b0 * *in;
    s1 = s2 + b1 * *in - a1 * *out ;
    s2 = b2 * *in - a2 * *out;
    in++;
    out++;
    i++;
    }
    
}

int Filter::reCalculate() {
    const double a = powf(10.0, (gain/40.0));
    const double w = 2 * M_PI* GainFreq/sampleRate;
    double ws=sin(w),wc=cos(w),alpha = ws / (2 * Q),a0;
    switch (filt) {
            case LOW_SHELF:
                a0 = (a + 1.0) + (a - 1.0) * wc + 2.0 * alpha * sqrt(a);
                b0 = (a * ((a + 1.0) - (a - 1.0) * wc + 2.0 * alpha * sqrt(a))) / a0;
                b1 = (2.0 * a * ((a - 1.0) - (a + 1.0) * wc)) / a0;
                b2 = (a * ((a + 1.0) - (a - 1.0) * wc - 2.0 * alpha * sqrt(a))) / a0;
                a1 = (-2.0 * ((a - 1.0) + (a + 1.0) * wc)) / a0;
                a2 = ((a + 1.0) + (a - 1.0) * wc - 2.0 * alpha * sqrt(a)) / a0;
                break;
            case HIGH_SHELF:
                a0 = (a + 1.0) - (a - 1.0) * wc + 2.0 * alpha * sqrt(a);
                b0 = (a * ((a + 1.0) + (a - 1.0) * wc + 2.0 * alpha * sqrt(a))) / a0;
                b1 = (-2.0 * a * ((a - 1.0) + (a + 1.0) * wc)) / a0;
                b2 = (a * ((a + 1.0) + (a - 1.0) * wc - 2.0 * alpha * sqrt(a))) / a0;
                a1 = (2.0 * ((a - 1.0) - (a + 1.0) * wc)) / a0;
                a2 = ((a + 1.0) - (a - 1.0) * wc - 2.0 * alpha * sqrt(a)) / a0;
                break;
            case PEAK:
                a0 = 1.0 + alpha / a;
                b0 = (1.0 + alpha * a) / a0;
                b1 = (-2.0 * wc) / a0;
                b2 = (1.0 - alpha * a) / a0;
                a1 = (-2.0 * wc) / a0;
                a2 = (1.0 - alpha / a) / a0;
                break;
            case ALL_PASS:
                a0 = 1.0 + alpha;
                b0 = (1.0 - alpha) / a0;
                b1 = (-2.0 * wc) / a0;
                b2 = (1.0 + alpha) / a0;
                a1 = (-2.0 * wc) / a0;
                a2 = (1.0 - alpha) / a0;
                break;
            default:
                cerr << "[FILT] No way! U just don't selected filter type!" << endl;
                return -1;
    }
    return 0;
}

void Filter::setGain(double gain) {
    this->gain = gain;
    this->reCalculate();
}

int Filter::setFilterType(FilterType filt) {
    this->filt = filt;
    return this->reCalculate();
}

void Filter::setGainFreq(unsigned int freq) {
    this->GainFreq = freq;
    this->reCalculate();
}

void Filter::setQValue(double Q) {
    this->Q = Q;
    this->reCalculate();
}

void Filter::setSampleRate(unsigned sampleRate) {
    this->sampleRate = sampleRate;
    this->reCalculate();
}
