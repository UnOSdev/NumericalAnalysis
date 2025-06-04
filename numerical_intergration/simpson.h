#ifndef INTEG_SIMPSON_H
#define INTEG_SIMPSON_H

#include <math.h>

double simpson_1_3_integration(double a, double b, const unsigned rects, double (*func)(double)){
    double h = fabs(b - a)/rects;
    double res = 0.0;
    double left = (a < b) ? a : b;
    for (size_t i = 0; i < rects + 1; i++)
    {
        if(i == 0 || i == rects){
            res += (*func)(left + (i * h));
        }else if(i % 2 == 0){
            res += 2 * (*func)(left + (i * h));
        }else{
            res += 4 * (*func)(left + (i * h));
        }
    }
    res *= h/3.0;
    return (res < 0) ? -res : res;
}


double simpson_3_8_integration(double a, double b, const unsigned n, double (*func)(double)){
    double diff = fabs(b - a);
    if(n == 1){
        double x1 = a + diff/3.0;
        double x2 = a + 2 * diff/3.0;
        double res = diff * ((*func)(a) + 3 * (*func)(x1) + 3 * (*func)(x2) + (*func)(b))/8.0;
        return (res < 0) ? -res : res;
    }else{
        double interval = diff/2;
        return simpson_3_8_integration(a, a + interval, n-1, func) + simpson_3_8_integration(a + interval, b, n-1, func);
    }
}
#endif