#include <stdio.h>
#include <math.h>

double bisection_meth(double x0, double x1, double eps, double (*func)(double))
{
    double res0 = (*func)(x0);
    double res1 = (*func)(x1);
    double pivot;

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
        printf("ERROR: Cannot detect a root between the intervals! (f(a) * f(b) is equal or bigger than 0)\n");
        return __INT64_MAX__ + 1;
    }
    do
    {
        pivot = (x0 + x1) / 2.0;
        double t = (*func)(pivot);
        if (t * res0 < 0)
            x1 = pivot;
        else if (t * res1 < 0)
        {
            x0 = pivot;
        }
        else
        {
            return pivot;
        }
        res0 = (*func)(x0);
        res1 = (*func)(x1);
#ifdef DEBUG
        printf("x0: %lf (%lf) x1: %lf (%lf) pivot: %lf (%lf)\n", x0, res0, x1, res1, pivot, t);
#endif
    } while (fabs(x0 - x1) > eps);
    return pivot;
}