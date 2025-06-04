#include <stdio.h>
#include <stdlib.h>

#include "closed_methods/bisection.h"
#include "closed_methods/regula_falsi.h"
#include "open_methods/newton_raphton.h"


#include "linear_equations/matrix.h"

#include "numerical_intergration/trapez.h"
#include "numerical_intergration/simpson.h"
#include "numerical_differentiation/num_deriv.h"

#include "linear_equations/gauss_seidal.h"
#include "linear_equations/cholesky.h"

#include "data_structures/general_data_structures.h"
#include "parser/parser.h"


/* Helper to compute factorial */
int fact(int n) {
    int res = 1;
    for(int i = 2; i <= n; i++)
        res *= i;
    return res;
}

/* Gregory-Newton Forward Interpolation */
double gregory_newton_interp(double *x, double *y, int n, double x0) {
    double h = x[1] - x[0];  // assume uniform spacing

    // Build forward difference table
    double diff[n][n];
    for(int i = 0; i < n; i++)
        diff[i][0] = y[i];

    for(int j = 1; j < n; j++)
        for(int i = 0; i < n - j; i++)
            diff[i][j] = diff[i+1][j-1] - diff[i][j-1];

    double p = (x0 - x[0]) / h;
    double result = y[0];
    double p_term = 1.0;

    for(int i = 1; i < n; i++) {
        p_term *= (p - (i - 1));
        result += (p_term * diff[0][i]) / fact(i);
    }

    return result;
}

typedef enum{
    BISECT = 1,
    REGUFA,
    NEWRAP,
    INVERS,
    CHOLSK,
    SEIDAL,
    DERIVE,
    SIMPSO,
    TRAPEZ,
    GRENEW,
    EXIT
} Choice;
int main(){
    printf("Welcome to Ubeyda's Calculator!\n");
    bool running = true;
    while(running){
        printf("1. Solve an equation using the 'Bisection Method'\n");
        printf("2. Solve an equation using the 'Regula-Falsi Method'\n");
        printf("3. Solve an equation using the 'Newton-Raphson Method'\n");
        printf("4. Get inverse of a matrix\n");
        printf("5. Solve a set of equations using the 'Cholesky Method'\n");
        printf("6. Solve a set of equations using the 'Gauss Seidal Method'\n");
        printf("7. Find the derivative of an equation\n");
        printf("8. Find the integral of an equation using 'Simpson Method'(1/3 and 1/8)\n");
        printf("9. Find the integral of an equation using 'Trapezoidal Rule Method'\n");
        printf("10. Interpolate an equation using 'Gregory Newton Method'\n");
        printf("11. Exit\n");
        printf("Make a choice: ");
        Choice input;
        scanf("%i", &input);
        getchar();

        char expr[256];
        char ans[4];    // enough to hold "Y\n\0"
        char cont = 'y';
        double x0, x1;
        char line[64];
        switch (input)
        {
            case BISECT:
                while (cont == 'y' || cont == 'Y') {
                    printf("Enter lower and upper bounds (e.g. -10 10): ");
                    if (!fgets(line, sizeof(line), stdin)) break;
                    if (sscanf(line, "%lf %lf", &x0, &x1) != 2) {
                        printf("Invalid input. Using default (-10, 10).\n");
                        x0 = -10; x1 = 10;
                    }
                    // 1) read the function
                    printf("Enter f(x): ");
                    fgets(expr, sizeof(expr), stdin);
                    expr[strcspn(expr, "\n")] = 0; 
                
                    // 2) parse & solve
                    double (*f)(double) = parse_function(expr);
                    double root = bisection_meth(x0, x1, 1e-8, f);
                    if(root != __INT64_MAX__ + 1) printf("x ~= %.8f\n", root);
                
                    // 3) ask to repeat
                    printf("Try another equation? (Y/n): ");
                    fgets(ans, sizeof(ans), stdin);
                    // take first non-whitespace character
                    cont = ans[0];
                    if (cont == '\n' || cont == '\0') 
                        cont = 'y';  // default to 'y' on empty line
                }
                break;
            case REGUFA: 
                while (cont == 'y' || cont == 'Y') {
                    printf("Enter lower and upper bounds (e.g. -10 10): ");
                    if (!fgets(line, sizeof(line), stdin)) break;
                    if (sscanf(line, "%lf %lf", &x0, &x1) != 2) {
                        printf("Invalid input. Using default (-10, 10).\n");
                        x0 = -10; x1 = 10;
                    }
                    // 1) read the function
                    printf("Enter f(x): ");
                    fgets(expr, sizeof(expr), stdin);
                    expr[strcspn(expr, "\n")] = 0; 
                
                    // 2) parse & solve
                    double (*f)(double) = parse_function(expr);
                    double root = regula_falsi(x0, x1, 1e-8, f);
                    if(root != __INT64_MAX__ + 1) printf("x ~= %.8f\n", root);
                
                    // 3) ask to repeat
                    printf("Try another equation? (Y/n): ");
                    fgets(ans, sizeof(ans), stdin);
                    // take first non-whitespace character
                    cont = ans[0];
                    if (cont == '\n' || cont == '\0') 
                        cont = 'y';  // default to 'y' on empty line
                }
                break;
            case NEWRAP:
                {
                    char buf[256];
                    const double eps = 1e-8;
                    double x0;
                    while (cont == 'y' || cont == 'Y') {
                        // 1) get the starting guess
                        printf("Enter initial guess x0: ");
                        fgets(buf, sizeof(buf), stdin);
                        sscanf(buf, "%lf", &x0);
                        
                        // 2) read & save f(x)
                        printf("Enter f(x): ");
                        fgets(buf, sizeof(buf), stdin);
                        buf[strcspn(buf, "\n")] = 0;
                        parse_function(buf);            // builds rpn[] for f
                        save_current_rpn(&rpn_f, &rpn_len_f);
                        
                        // 3) read & save f'(x)
                        printf("Enter f'(x): ");
                        fgets(buf, sizeof(buf), stdin);
                        buf[strcspn(buf, "\n")] = 0;
                        parse_function(buf);            // rebuilds rpn[] for f′
                        save_current_rpn(&rpn_fder, &rpn_len_fder);
                        
                        // 4) call Newton–Raphson with our wrappers
                        double root = newton_raphton(x0, eps, call_f, call_fder);
                        if (root != __INT64_MAX__ + 1)
                            printf("x ~= %.8f\n", root);
                        // 3) ask to repeat
                        printf("Try another equation? (Y/n): ");
                        fgets(ans, sizeof(ans), stdin);
                        // take first non-whitespace character
                        cont = ans[0];
                        if (cont == '\n' || cont == '\0') 
                            cont = 'y';  // default to 'y' on empty line
                    }
                }
                break;
                
                case INVERS:
                    while (cont == 'y' || cont == 'Y') {
                        
                        // Inverse of an n×n matrix
                        char line[64];
                        printf("Enter matrix size n: ");
                        fgets(line, sizeof(line), stdin);
                        int n = atoi(line);
                    
                        Matrix A = initMatrix(n, n);
                        double flat[n * n];
                        printf("Enter %dx%d entries row-by-row:\n", n, n);
                        for (int i = 0; i < n; i++) {
                            fgets(line, sizeof(line), stdin);
                            char *p = line;
                            for (int j = 0; j < n; j++) {
                                flat[i*n + j] = strtod(p, &p);
                            }
                        }
                        // now populate the Matrix in one go
                        setDataMatrix(A, flat);
                    
                        Matrix Ainv = inverse(A);
                        printf("Inverse matrix:\n");
                        printMatrix(Ainv);
                    
                        freeMatrix(A);
                        freeMatrix(Ainv);
                        printf("Try another matrix? (Y/n): ");
                        fgets(ans, sizeof(ans), stdin);
                        // take first non-whitespace character
                        cont = ans[0];
                        if (cont == '\n' || cont == '\0') 
                            cont = 'y';  // default to 'y' on empty line
                    }
                break;

            case CHOLSK:
                while (cont=='y'||cont=='Y'){
                    //printf("Enter size n and then %dx%dx1 constant vector:\n");
                    // read n and constants and matrix then:
                    printf("Enter matrix size n: ");
                    fgets(line, sizeof(line), stdin);
                    int m = atoi(line);
                    Matrix A = initMatrix(m,m);
                    Matrix bvec = initMatrix(m,1);
                    double temp;
                    printf("Enter %d rows of %d entries for A:\n", m, m);
                    for(int i=0;i<m;i++){
                        fgets(line,sizeof(line),stdin);
                        char* p=line;
                        for(int j=0;j<m;j++){
                            A.data[i][j]=strtod(p,&p);
                        }
                    }
                    bool is_symmetric = true;
                    for(int i=0;i<m;i++)
                      for(int j=0;j<m;j++)
                        if (fabs(A.data[i][j] - A.data[j][i]) > 1e-12)
                          is_symmetric = false;
                    if (!is_symmetric) {
                      printf("Error: matrix must be symmetric positive-definite for Cholesky.\n");
                    }else{
                        printf("Enter %d entries for b:\n", m);
                        for(int i=0;i<m;i++){
                            fgets(line,sizeof(line),stdin);
                            bvec.data[i][0]=strtod(line,NULL);
                        }
                        printf("DEBUG: A =\n"); printMatrix(A);
                        printf("DEBUG: b =\n"); printMatrix(bvec);
                        Matrix x = cholesky(A,bvec,1e-8);
                        printf("Solution vector x:\n");
                        printMatrix(x);
                        freeMatrix(A); freeMatrix(bvec); freeMatrix(x);
                        }
                    printf("Try another? (Y/n): "); fgets(ans,sizeof(ans),stdin);
                    cont=ans[0]==0?'y':ans[0];
                }
                break;
            case SEIDAL:
                while(cont=='y'||cont=='Y'){
                    printf("Enter matrix size n and then augmented matrix rows (A|b):\n");
                    fgets(line,sizeof(line),stdin);
                    int m = atoi(line);
                    Matrix A=initMatrix(m,m);
                    Matrix bvec=initMatrix(m,1);
                    printf("Enter each row with %d+1 entries (A row then b):\n",m);
                    for(int i=0;i<m;i++){
                        fgets(line,sizeof(line),stdin);
                        char* p = line;
                        for(int j=0;j<m;j++) 
                            A.data[i][j]=strtod(p,&p);
                        bvec.data[i][0]=strtod(p,NULL);
                    }
                    Matrix sol = gauss_seidal(A,bvec,1e-8);
                    printf("Solution vector x:\n"); printMatrix(sol);
                    freeMatrix(A); freeMatrix(bvec); freeMatrix(sol);
                    printf("Try another? (Y/n): "); fgets(ans,sizeof(ans),stdin);
                    cont=ans[0]==0?'y':ans[0];
                }
                break;
            case DERIVE:
                while(cont=='y'||cont=='Y'){
                    printf("Enter function f(x): ");
                    double a, b;
                    fgets(expr,sizeof(expr),stdin); expr[strcspn(expr,"\n")]=0;
                    double (*f)(double)=parse_function(expr);
                    for (int i = 0; i < rpn_len; ++i)
                      printf("%s ", rpn[i]);
                    printf("\n");
                    printf("Enter point x0 and epsilon (Ex: 3.53 2.321): ");
                    fgets(line,sizeof(line),stdin); sscanf(line,"%lf %lf",&x0,&a);
                    double d = central_diff_derivative(x0,a,f);
                    printf("f'(%.4f) ~= %.8f\n", x0, d);
                    printf("Try another? (Y/n): "); fgets(ans,sizeof(ans),stdin);
                    cont=ans[0]==0?'y':ans[0];
                }
                break;

            case SIMPSO:
                while(cont=='y'||cont=='Y'){
                    printf("Enter a b and number of subintervals n: ");
                    double a, b; 
                    unsigned n;
                    fgets(line,sizeof(line),stdin); sscanf(line,"%lf %lf %u",&a,&b,&n);
                    printf("Enter function f(x): "); fgets(expr,sizeof(expr),stdin); expr[strcspn(expr,"\n")]=0;
                    double (*f2)(double)=parse_function(expr);
                    double res1 = simpson_1_3_integration(a,b,n,f2);
                    double res2 = simpson_3_8_integration(a,b,n,f2);
                    printf("Simpson 1/3 result: %.8f\n", res1);
                    printf("Simpson 3/8 result: %.8f\n", res2);
                    printf("Try another? (Y/n): "); fgets(ans,sizeof(ans),stdin);
                    cont=ans[0]==0?'y':ans[0];
                }
                break;

            case TRAPEZ:
                while(cont=='y'||cont=='Y'){
                    double a, b;
                    unsigned n;
                    printf("Enter a b and number of trapezoids n: ");
                    fgets(line,sizeof(line),stdin); sscanf(line,"%lf %lf %u",&a,&b,&n);
                    printf("Enter function f(x): "); fgets(expr,sizeof(expr),stdin); expr[strcspn(expr,"\n")]=0;
                    double (*f3)(double)=parse_function(expr);
                    double res = trapezoidal_integration(a,b,n,f3);
                    printf("Trapezoidal result: %.8f\n", res);
                    printf("Try another? (Y/n): "); fgets(ans,sizeof(ans),stdin);
                    cont=ans[0]==0?'y':ans[0];
                }
                break;
            case GRENEW: 
                while(cont == 'y' || cont == 'Y'){                
                    int n;
                    printf("Enter number of data points: ");
                    scanf("%d", &n);
                    
                    double x[n], y[n];
                    printf("Enter x values:\n");
                    for(int i = 0; i < n; i++) scanf("%lf", &x[i]);
                    printf("Enter y values:\n");
                    for(int i = 0; i < n; i++) scanf("%lf", &y[i]);
                    
                    double x0;
                    printf("Enter x0 to interpolate: ");
                    scanf("%lf", &x0);
                    
                    double y0 = gregory_newton_interp(x, y, n, x0);
                    printf("Interpolated value at x = %.4f is %.8f\n", x0, y0);
                    printf("Try another? (Y/n): "); scanf(" %c", ans);
                    cont=ans[0]==0?'y':ans[0];
                }
                break;
            case EXIT:
                running = false;
                break;
            default:
                printf("Unknown input!\n");
                break;
        }
    }
    




    
    return 0;
}
