/* Copyright (C) 2020 Oleg Sazonov */
#include <iostream>
#include <cmath>
#include "Filter.hh"

Filter::Filter(int GainFreq, int gain, enum FilterType filt, unsigned smpl, double Q) {
    const double a = powf(10.0, (gain/40.0));
    const double w = 2 * M_PI* GainFreq/smpl;
    double ws=sin(w),wc=cos(w);
    double alpha = ws / (2 * Q);
    switch (filt) {
            case LOW_SHELF:
                b0 = a * ((a + 1.0) - (a - 1.0) * wc + 2.0 * alpha * sqrt(a));
                b1 = 2.0 * a * ((a - 1.0) - (a + 1.0) * wc);
                b2 = a * ((a + 1.0) - (a - 1.0) * wc - 2.0 * alpha * sqrt(a));
                a0 = (a + 1.0) + (a - 1.0) * wc + 2.0 * alpha * sqrt(a);
                a1 = -2.0 * ((a - 1.0) + (a + 1.0) * wc);
                a2 = (a + 1.0) + (a - 1.0) * wc - 2.0 * alpha * sqrt(a);
                break;
            case HIGH_SHELF:
                b0 = a * ((a + 1.0) + (a - 1.0) * wc + 2.0 * alpha * sqrt(a));
                b1 = -2.0 * a * ((a - 1.0) + (a + 1.0) * wc);
                b2 = a * ((a + 1.0) + (a - 1.0) * wc - 2.0 * alpha * sqrt(a));
                a0 = (a + 1.0) - (a - 1.0) * wc + 2.0 * alpha * sqrt(a);
                a1 = 2.0 * ((a - 1.0) - (a + 1.0) * wc);
                a2 = (a + 1.0) - (a - 1.0) * wc - 2.0 * alpha * sqrt(a);
                break;
            case PEAK:
                b0 = 1.0 + alpha * a;
                b1 = -2.0 * wc;
                b2 = 1.0 - alpha * a;
                a0 = 1.0 + alpha / a;
                a1 = -2.0 * wc;
                a2 = 1.0 - alpha / a;
                break;
            default:
                cerr << "[FILT] No way! U just don't selected filter type!" << endl;
                exit(0);      
    }
}

void Filter::clear() {
    x1 = x2 = y1 = y2 = 0;
}

double Filter::process(double in) {
    double output;

    output = (b0 / a0) * in + (b1 / a0) * x1 + (b2 / a0) * x2 - (a1 / a0) * y1 - (a2 / a0) * y2;
    x2 = x1;
    x1 = in;
    y2 = y1;
    y1 = output;
    return output;
}
