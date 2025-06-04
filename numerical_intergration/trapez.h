#include <math.h>
#include <stdio.h>

double trapezoidal_integration(double a, double b, unsigned rects, double (*func)(double)){
    double width = fabs(b - a) / (double)rects;
    double result = 0.0;
    for (size_t i = 1; i <= rects; i++)
    {

        double h0 = (*func)(a + (width * (double)(i - 1)));
        double h1 = (*func)(a + (width * (double)(i)));
        result += 0.5f * width * (h0 + h1);        
    }
    return result;
}

double trapezoidal_integration_debug(double a, double b, unsigned rects, double (*func)(double)){
    double width = fabs(b - a) / (double)rects;
    printf("WIDTH: %lf\n", width);
    double result = 0.0;
    for (size_t i = 1; i <= rects; i++)
    {

        double h0 = (*func)(a + (width * (double)(i - 1)));
        double h1 = (*func)(a + (width * (double)(i)));
        result += 0.5f * width * (h0 + h1);
        printf("h0: %lf\th1:%lf\tres:%lf\n", h0, h1, result);
        
    }
    return result;
}



double trapezoidal_integration_improved(double a, double b, unsigned rects, double (*func)(double)){
    double width = fabs(b - a) / (double)rects;
    double result = 0.0;
    for (size_t i = 1; i <= rects - 1; i++)
    {
        result += (*func)(a + (width * (double)i));
    }
    double h0 = (*func)(a);
    double hn = (*func)(a + (width * (double)(rects)));
    result = (result + (h0 + hn) * 0.5) * width;
    return result;
}

double trapezoidal_integration_improved_debug(double a, double b, unsigned rects, double (*func)(double)){
    double width = fabs(b - a) / (double)rects;
    printf("WIDTH: %lf\n", width);
    double result = 0.0;
    for (size_t i = 1; i <= rects - 1; i++)
    {
        double value = (*func)(a + (width * (double)i));
        result += value;
        printf("result: %lf\tvalue:%lf\n", result, value);
    }
    double h0 = (*func)(a);
    double hn = (*func)(a + (width * (double)(rects)));
    result = (result + (h0 + hn) * 0.5) * width;
    printf("h0: %lf\thn:%lf\tres:%lf\n", h0, hn, result);
    return result;
}