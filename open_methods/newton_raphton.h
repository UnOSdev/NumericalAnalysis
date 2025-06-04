#include <math.h>
#include <stdio.h>
// Using the derivative of f(x) we can find the solution
// By approaching to the answer

// Formula: 
//          x1 = x0 - ( f(x0) / f'(x0) )
// Return: 
//          x1 if f(x1) < eps
double newton_raphton(double x, double eps, double (*func)(double), double (*deriv_func)(double)){
    double res = (*func)(x);
    if(fabs(res) <= eps){
        return x;
    }
    do
    {
        x = x - res/(*deriv_func)(x);
        res = (*func)(x);
    } while (fabs(res) > eps);
    return x;
}

double newton_raphton_debug(double x, double eps, double (*func)(double), double (*deriv_func)(double)){
    static unsigned iterations = 0;
    double res = (*func)(x);
    if(fabs(res) <= eps){
        return x;
    }
    do
    {
        iterations++;
        printf("BEFORE:\tx: (%lf) f(x): %lf f'(x): %lf\n", x, res, (*deriv_func)(x));
        x = x - res/(*deriv_func)(x);
        res = (*func)(x);
        printf("AFTER:\tx: (%lf) f(x): %lf f'(x): %lf\n", x, res, (*deriv_func)(x));
    } while (fabs(res) > eps);
    printf("Iterations: %u\n\n", iterations);
    return x;
}

