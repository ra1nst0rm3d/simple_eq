/* Copyright (C) 2020 Oleg Sazonov */
#include <iostream>
#include <cmath>
#include "Filter.hh"

Filter::Filter(int GainFreq, int gain, enum FilterType filt, unsigned smpl, double Q) {
    const double a = powf(10.0, (gain/40.0));
    const double w = 2 * M_PI* GainFreq/smpl;
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
                exit(0);
    }
}
void Filter::clear() {
    s1 = s2 = 0;
}

double Filter::process(double in) {
    double output = s1 + b0 * in;
    s1 = s2 + b1 * in - a1 * output;
    s2 = b2 * in - a2 * output;


    return output;
}
