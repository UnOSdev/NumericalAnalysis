#include <stdio.h>
#include <math.h>
#include <monetary.h>

double graph_meth(double x0, double dx, double eps, double (*func)(double))
{
    if (dx < eps)
    {
        printf("dx smaller than eps!\n");
        return __INT64_MAX__ + 1; // Overflowing on purpose
    }
    while (fabs(dx) > eps)
    {
        // usleep(100);
        double res0 = (*func)(x0);
        double res1 = (*func)(x0 + dx);
        if (res0 == 0)
            return x0;
        else if (res1 == 0)
            return x0 + dx;
        else if ((res0 * res1) <= 0)
            dx /= 2.0f;
        else
            x0 += dx;
    }
    return x0;
}

double graph_meth_debug(double x0, double dx, double eps, double (*func)(double))
{
    if (dx < eps)
    {
        printf("dx smaller than eps!\n");
        return __INT64_MAX__ + 1; // Overflowing on purpose
    }
    while (fabs(dx) > eps)
    {
        // usleep(100);
        double res0 = (*func)(x0);
        double res1 = (*func)(x0 + dx);
        if (res0 == 0)
        {
            return x0;
        }
        else if (res1 == 0)
        {
            return x0 + dx;
        }
        else if ((res0 * res1) <= 0)
        {
            dx /= 2.0f;
            printf("x0: %lf x1: %lf dx: %lf (RANGE)\n", x0, x0 + dx, dx);
            continue;
        }
        printf("x0: %lf x1: %lf dx: %lf\n", x0, x0 + dx, dx);
        x0 += dx;
    }
    return x0;
}