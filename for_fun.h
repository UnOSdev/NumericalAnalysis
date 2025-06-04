#include <stdio.h>
#include <math.h>
double my_sqrt(double value){
    double x0 = 0;
    double x1 = value;
    double eps = 1e-10;

    double res0 = (x0) * (x0) - value;
    double res1 = (x1) * (x1) - value;
    ///double res0 = -;
    //double res1 = (*func)(x1);

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
        res = pos * pos - value;
        //printf("pos: %lf\tres:%lf\tx0:%lf\tx1:%lf\tres0:%lf\tres1:%lf\n", pos, value, x0, x1, res0, res1);
        if(res * x0 < 0){
            x1 = pos;
            res1 = res;
        }else{
            x0 = pos;
            res0 = res;
        }
    } while (fabs(res1) >= eps);
    return pos;
}