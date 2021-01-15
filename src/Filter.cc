/* Copyright (C) 2020 Oleg Sazonov */
#include <iostream>
#include <cmath>
#include "Filter.hh"

Filter::Filter(int GainFreq, int gain, enum FilterType filt, unsigned smpl, double Q) {
    const double A = sqrt(pow(10.0, (gain)/20.0));
    const double w = 2 * M_PI* GainFreq/smpl;
    const double alpha = sin(w)/(2 * Q);
    switch (filt) {
            case LOW_SHELF:
                b0 = A * ((A+1) - (A-1)*cos(w) + 2*sqrt(A)*alpha);
                b1 = 2*A * ((A-1) - (A+1)*cos(w));
                b2 = A * ((A+1) - (A-1)*cos(w) - 2*sqrt(A)*alpha);
                a0 = (A+1) + ((A-1)*cos(w)) + 2*sqrt(A)*alpha;
                a1 = -2 * ((A-1) + (A+1)*cos(w));
                a2 = (A+1) + (A-1)*cos(w) - 2*sqrt(A)*alpha;
                break;
            case HIGH_SHELF:
                b0 = A * ((A+1) + (A-1)*cos(w) + 2*sqrt(A)*alpha);
                b1 = -2*A * ((A-1) + (A+1)*cos(w));
                b2 = A * ((A+1) + (A-1)*cos(w) - 2*sqrt(A)*alpha);
                a0 = (A+1) - ((A-1)*cos(w)) + 2*sqrt(A)*alpha;
                a1 = 2 * ((A-1) - (A+1)*cos(w));
                a2 = (A+1) - (A-1)*cos(w) - 2*sqrt(A)*alpha;
                break;
            default:
            cerr << "[FILT] No way! U just don't selected filter type!" << endl;
            exit(0);      
    }
    this->filt = filt;
    this->sample_freq = smpl;
}

double* Filter::process(double *in, size_t size) {
    double* output = (double*)malloc(sizeof(double) * size);
    double bin1 = 0, bin2 = 0, bout1 = 0, bout2 = 0;
    for(int i = 0; i < sizeof(output)/sizeof(double); i++) {
        *(output + i) = (b0 / a0) * *(in + i) + (b1 / a0) * bin1 + (b2 / a0) * bin2 - (a1 / a0) * bout1 - (a2 / a0) * bout2;
        bin2 = bin1;
        bin1 = *(in + i);
        bout2 = bout1;
        bout1 = *(output + i);
    }
    return output;
}
