#if !defined(CHOLESKY_H)
#define CHOLESKY_H

#include "matrix.h"

Matrix cholesky(const Matrix equations, const Matrix constants, double eps){
    if (equations.rows != equations.cols || equations.rows != constants.rows || constants.cols != 1) {
        printf("Matrix dimension mismatch in cholesky()!\n");
        return (Matrix){.rows = 0, .cols = 0, .data = NULL};
    }

    unsigned n = equations.rows;
    Matrix L = initMatrix(n, n); // Alt üçgen matris
    Matrix y = initMatrix(n, 1);
    Matrix x = initMatrix(n, 1);

    // Cholesky Ayrıştırması: A = L * L^T
    for (unsigned i = 0; i < n; i++) {
        for (unsigned j = 0; j <= i; j++) {
            double sum = 0.0;
            for (unsigned k = 0; k < j; k++)
                sum += L.data[i][k] * L.data[j][k];

            if (i == j)
                L.data[i][j] = sqrt(fmax(equations.data[i][i] - sum, eps));
            else
                L.data[i][j] = (1.0 / L.data[j][j]) * (equations.data[i][j] - sum);
        }
    }

    // İleri Yerine Koyma: L * y = b
    for (unsigned i = 0; i < n; i++) {
        double sum = 0.0;
        for (unsigned k = 0; k < i; k++)
            sum += L.data[i][k] * y.data[k][0];
        y.data[i][0] = (constants.data[i][0] - sum) / L.data[i][i];
    }

    // Geri Yerine Koyma: L^T * x = y
    for (int i = n - 1; i >= 0; i--) {
        double sum = 0.0;
        for (unsigned k = i + 1; k < n; k++)
            sum += L.data[k][i] * x.data[k][0];
        x.data[i][0] = (y.data[i][0] - sum) / L.data[i][i];
    }

    freeMatrix(L);
    freeMatrix(y);
    return x;
}



#endif // CHOLESKY_H
