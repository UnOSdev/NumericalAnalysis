#include <math.h>
#include <stdio.h>

// Basically same as newton-raphton, 
// Except that we approximate the derivative
// To find the solution

// Formula: 
//          k = (N - 1)th x -> (x-1)
//          f'(x0) = f(k) - f(x0) / (k - x0)
//          x1 = x0 - ( f(x0) / f'(x0) )
//          x1 = x0 - ( f(x0) * [k - x0] ) / ( f(k) - f(x0) )
// Return: 
//          x1 if f(x1) < eps

double secant(double x, double eps, double (*func)(double)){
    double res = (*func)(x);
    if(fabs(res) <= eps){
        return x;
    }
    
    double x_old = (x - 1);
    double res_old = (*func)(x_old);

    do
    {
        double deriv = (res_old - res ) / (x_old - x);
        x_old = x;
        res_old = res;
        x = x - ( res / deriv );
        res = (*func)(x);
        
    } while (fabs(res) > eps);
    return x;
}

double secant_debug(double x, double eps, double (*func)(double)){
    static unsigned iterations = 0;
    double res = (*func)(x);
    if(fabs(res) <= eps){
        return x;
    }
    
    double x_old = (x - 1);
    double res_old = (*func)(x_old);

    do
    {
        iterations++;
        double deriv = (res_old - res ) / (x_old - x);
        printf("BEFORE:\tx: (%lf) f(x): %lf f'(x): %lf\n", x, res, deriv);
        x_old = x;
        res_old = res;
        x = x - ( res / deriv );
        res = (*func)(x);
        printf("AFTER:\tx: (%lf) f(x): %lf f'(x): %lf\n", x, res, deriv);
        
    } while (fabs(res) > eps);
    printf("Iterations: %u\n\n", iterations);
    return x;
}