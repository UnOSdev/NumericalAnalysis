#ifndef GAUSS_SEIDAL_H
#define GAUSS_SEIDAL_H

#include "matrix.h"

Matrix gauss_seidal(const Matrix equations, const Matrix constants, double eps){
    Matrix *matrices;
    Matrix solutions = initMatrix(equations.rows, 1);
    double *deltas;
    double *biggers;

    Matrix copy_eque = copyMatrix(equations);
    Matrix copy_cons = copyMatrix(constants);
    for (size_t i = 0; i < copy_eque.cols; i++)
    {
        bool swapped = true;
        for (size_t j = 0; (j < copy_eque.rows - 1) && (swapped == true); j++)
        {
            swapped = false;
            for (size_t k = j; k < copy_eque.rows - 1 - j; k++)
            {
                if(copy_eque.data[j][k] < copy_eque.data[j][k + 1]){
                    swapRows(copy_eque, k, k + 1);
                    swapRows(copy_cons, k, k + 1);
                    swapped = true;
                }
            }
        }
    }
    

    matrices = malloc(sizeof(Matrix) * copy_eque.rows);
    deltas = malloc(sizeof(double) * copy_eque.rows);
    biggers = malloc(sizeof(double) * copy_eque.rows);

    for (size_t i = 0; i < copy_eque.rows; i++)
    {
        // Initialize matrices
        Matrix mat = initMatrix(1, copy_eque.cols);
        for (size_t j = 0; j < copy_eque.cols; j++)
            mat.data[0][j] = copy_eque.data[i][j];

        unsigned *ref;
        matrices[i].data = mat.data;
        ref = &matrices[i].rows;
        *ref = mat.rows;
        ref = &matrices[i].cols;
        *ref = mat.cols;

        // Find the biggest 
        biggers[i] = matrices[i].data[0][i];
    
        matrices[i].data[0][i] = 0;
        scalarMultiply(matrices[i], -1);
        // Initialize solutions
        solutions.data[i][0] = 1;
    }
    

    bool condition;
    do{
        for (size_t i = 0; i < copy_eque.rows; i++)
        {
            double res = multiply(matrices[i], solutions).data[0][0];
            res += copy_cons.data[i][0];
            res /= biggers[i];
            deltas[i] = fabs(solutions.data[i][0] - res);
            solutions.data[i][0] = res;
        }
        condition = false;
        for (size_t i = 0; (i < copy_eque.rows && condition == false); i++)
            if(deltas[i] > eps)
                condition = true;
        
    }while(condition);
    
    for (size_t i = 0; i < copy_eque.rows; i++)
    {
        freeMatrix(matrices[i]);
    }
    freeMatrix(copy_eque);
    free(matrices);
    free(deltas);
    free(biggers);
    

    return solutions;
}

#endif