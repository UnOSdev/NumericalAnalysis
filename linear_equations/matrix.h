#ifndef MATRIX_H
#define MATRIX_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

typedef struct sMat{
    double **data;
    unsigned rows;
    unsigned cols;
} Matrix;

int ipow(int num, unsigned pow){
    int res = 1;
    while(pow-- > 0) res*=num;
    return res;
}

void fswap(double *x, double *y){
    double t = *x;
    *x = *y;
    *y = t;
}

void swap(int *x, int *y){
    int t = *x;
    *x = *y;
    *y = t;
}

Matrix swapRows(Matrix mat, unsigned row1, unsigned row2){
    if(row1 >= mat.rows || row2 >= mat.rows){
        printf("Row index exceding boundaries when swapping! Empty matrix returned\n");
        return (Matrix){
            .cols = 0,
            .rows = 0,
            .data = NULL
        };
    }
    
    for (size_t i = 0; i < mat.cols; i++)
        fswap(&mat.data[row1][i], &mat.data[row2][i]);
    return mat;
} 

Matrix initMatrix(unsigned row, unsigned col){
    Matrix mat;
    mat.data = malloc(sizeof(double*) * row);
    for (size_t i = 0; i < row; i++)
    {
        mat.data[i] = calloc(col, sizeof(double));
    }
    unsigned *ref = &mat.rows; 
    *ref = row;
    ref = &mat.cols;
    *ref = col;
    return mat;
}

void setDataMatrix(Matrix mat, double *data){
    for (size_t i = 0; i < mat.rows; i++)
        for (size_t j = 0; j < mat.cols; j++)
            mat.data[i][j] = data[(i * mat.cols) + j];
}

void freeMatrix(Matrix mat){
    for (size_t i = 0; i < mat.rows; i++)
        free(mat.data[i]);
    free(mat.data);
}

void printMatrix(Matrix mat){
    for (size_t i = 0; i < mat.rows; i++)
    {
        printf("| ");
        for (size_t j = 0; j < mat.cols; j++)
            printf("%.3lf ", mat.data[i][j]);
        printf("\t|\n");
    }
    printf("\n");
}

Matrix copyMatrix(Matrix mat){
    Matrix ret = initMatrix(mat.rows, mat.cols);
    for (size_t i = 0; i < mat.rows; i++)
        for (size_t j = 0; j < mat.cols; j++)
            ret.data[i][j] = mat.data[i][j];
    return ret;
}

void scalarMultiply(Matrix mat, double num){
    for (size_t i = 0; i < mat.rows; i++)
        for (size_t j = 0; j < mat.cols; j++)
            mat.data[i][j] *= num;
}

void scalarDivide(Matrix mat, double num){
    for (size_t i = 0; i < mat.rows; i++)
        for (size_t j = 0; j < mat.cols; j++)
            mat.data[i][j] /= num;
}

Matrix add(Matrix mat1, Matrix mat2){
    if(mat1.data == NULL || mat2.data == NULL || mat1.rows != mat2.rows || mat1.cols != mat2.cols){
        printf("WARNING: data == NULL OR dimensions mismatch for add() => Empty matrix returned!\n");
        return (Matrix){.rows = 0, .cols = 0, .data = NULL};
    }
    Matrix ret = copyMatrix(mat1);
    for (size_t i = 0; i < mat1.rows; i++)
        for (size_t j = 0; j < mat1.cols; j++)
            ret.data[i][j] += mat2.data[i][j];
    return ret;
}

Matrix subtract(Matrix mat1, Matrix mat2){
    if(mat1.data == NULL || mat2.data == NULL || mat1.rows != mat2.rows || mat1.cols != mat2.cols){
        printf("WARNING: data == NULL OR dimensions mismatch for subtract() => Empty matrix returned!\n");
        return (Matrix){.rows = 0, .cols = 0, .data = NULL};
    }
    Matrix ret = copyMatrix(mat1);
    for (size_t i = 0; i < mat1.rows; i++)
        for (size_t j = 0; j < mat1.cols; j++)
            ret.data[i][j] -= mat2.data[i][j];
    return ret;
}

Matrix multiply(Matrix mat1, Matrix mat2){
    if(mat1.cols != mat2.rows){
        printf("WARNING: dimensions mismatch for multiply() => Empty matrix returned!\n");
        return (Matrix){.rows = 0, .cols = 0, .data = NULL};
    }
    Matrix ret = initMatrix(mat1.rows, mat2.cols);
    double res = 0.0;
    for (size_t y = 0; y < ret.rows; y++)
    {
        for (size_t x = 0; x < ret.cols; x++)
        {
            res = 0.0;
            for (size_t i = 0; i < mat1.cols; i++)
                res += mat1.data[y][i] * mat2.data[i][x];
            ret.data[y][x] = res;
        }

    }
    return ret;
}

Matrix transpose(Matrix mat){
    Matrix ret_mat = initMatrix(mat.cols, mat.rows);
    for (size_t i = 0; i < mat.rows; i++)
        for (size_t j = 0; j < mat.cols; j++)
            ret_mat.data[j][i] = mat.data[i][j];
    return ret_mat;
}

Matrix submat(Matrix mat, unsigned i, unsigned j){
    if (i >= mat.rows || j >= mat.cols || mat.data == NULL) {
        fprintf(stderr, "WARNING: submat indices out of range! (%u,%u) in %ux%u\n", i, j, mat.rows, mat.cols);
        return (Matrix){0,0,NULL};
    }
    Matrix ret = initMatrix(mat.rows - 1, mat.cols - 1);
    unsigned y = 0, x;
    for (unsigned ii = 0; ii < mat.rows; ii++) {
        if (ii == i) continue;
        x = 0;
        for (unsigned jj = 0; jj < mat.cols; jj++) {
            if (jj == j) continue;
            ret.data[y][x++] = mat.data[ii][jj];
        }
        y++;
    }
    return ret;
}


double determinant(Matrix mat){
    if(mat.cols != mat.rows){
        printf("WARNING: rows != columns for determinant() => Overflow returned\n");
        return __INT64_MAX__ + 1;
    }
    if (mat.cols == 1) {
        return mat.data[0][0];
    }
    if(mat.cols == 2){
        return (mat.data[0][0] * mat.data[1][1]) - (mat.data[0][1] * mat.data[1][0]);
    }
    double res = 0.0f;
    for (size_t j = 0; j < mat.cols; j++)
    {
        Matrix sub = submat(mat, 0, j);
        res += ipow(-1, j) * mat.data[0][j] * determinant(sub);
        freeMatrix(sub);
    }
    return res;
}

Matrix cofactor(Matrix mat){
    Matrix ret = initMatrix(mat.rows, mat.cols);
    for (size_t i = 0; i < mat.rows; i++)
    {
        for (size_t j = 0; j < mat.cols; j++)
        {
            Matrix sub = submat(mat, i, j);
            ret.data[i][j] = ipow(-1, i+j) * determinant(sub);
            freeMatrix(sub);
        }   
    }
    return ret;
}

Matrix adjoint(Matrix mat){
    Matrix cof = cofactor(mat);
    Matrix ret = transpose(cof);
    freeMatrix(cof);
    return ret;
}

Matrix inverse(Matrix mat){
    Matrix adj = adjoint(mat);
    double deter = determinant(mat);
    scalarDivide(adj, deter);
    return adj;
}

#endif