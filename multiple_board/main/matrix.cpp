/**
 * @file matrix.cpp
 * 
 * @brief Float matrix operations
 */

#include "matrix.hpp"

void sum(float* A, float* B, float* C, int n, int m)
{
    int i, j;
    for (i = 0; i < n; i++){
        for(j = 0; j < m; j++){
            C[m * i + j] = A[m * i + j] + B[m * i + j];
        }
    }
}

void sub(float* A, float* B, float* C, int n, int m)
{
    int i, j;
    for (i = 0; i < n; i++){
        for(j = 0; j < m; j++){
            C[m * i + j] = A[m * i + j] - B[m * i + j];
        }
    }
}

void mul(float *A, float *B, float *C, int n, int m, int p)
{
    int i, j, k;
    for (i = 0; i < n; i++){
        for(j = 0; j < p; j++){
            C[p * i + j] = 0.0;
            for (k = 0; k < m; k++){
                C[p * i + j] += A[m * i + k] * B[p * k + j];
            }
        }
    }
}

void elemMul(float *A, float *B, float *C, int n, int m)
{
    int i, j;
    for (i = 0; i < n; i++){
        for(j = 0; j < m; j++){
            C[m * i + j] = A[m * i + j] * B[m * i + j];
        }
    }
}

void fill(float *A, float value, int n, int m)
{
    int length = n * m;
    for (int i = 0; i < length; i++){
        A[i] = value;
    }
}

void copy(float *A, float *B, int n, int m)
{
    memcpy(B, A, sizeof(float) * n * m);
}

void transpose(float *A, float *A_transpose, int n, int m)
{
    int i, j;

    for (i = 0; i < m; i++){
        for (j = 0; j < n; j++){
            A_transpose[j*n + i] = A[i*m + j];
        }
    }

}