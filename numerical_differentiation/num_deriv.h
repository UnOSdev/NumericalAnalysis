#include <math.h>

double back_diff_derivative(double x, double eps, double (*func)(double)){
    // ((*func)(x) - (*func)(x - eps)) / (x - (x - eps));
    return ((*func)(x) - (*func)(x - eps)) / (eps);
}

double front_diff_derivative(double x, double eps, double (*func)(double)){
    // ((*func)(x) - (*func)(x + eps)) / (x - (x + eps));
    return ((*func)(x + eps) - (*func)(x)) / (eps);
}

double central_diff_derivative(double x, double eps, double (*func)(double)){
    return ((*func)(x + eps) - (*func)(x - eps)) / (2.0 * eps);
}