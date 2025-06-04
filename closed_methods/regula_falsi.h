#include <math.h>
#include <stdio.h>

double regula_falsi(double x0, double x1, double eps, double (*func)(double))
{
    double res0 = (*func)(x0);
    double res1 = (*func)(x1);

    if (fabs(res0) <= eps)
    {
        return x0;
    }
    else if (fabs(res1) <= eps)
    {
        return x1;
    }
    else if (res0 * res1 >= 0)
    {
        printf("ERROR: Cannot detect a root between the intervals! (f(a) * f(b) is equal or bigger than 0)");
        return __INT64_MAX__ + 1;
    }
    double pos;
    double res;

    do
    {
        pos = (x0 * res1 - x1 * res0)/(res1 - res0);
        res = (*func)(pos);
        if(res * x0 < 0){
            x1 = pos;
            res1 = res;
        }else{
            x0 = pos;
            res0 = res;
        }
    } while (fabs(x1 - x0) > eps);
    return pos;
}