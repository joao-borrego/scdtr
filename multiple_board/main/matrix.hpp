/**
 * @file    multiple_board/main/matrix.cpp
 * 
 * @brief   Float matrix operations implementation
 * 
 * @author  João Borrego
 */

#ifndef INO_MATRIX_HPP
#define INO_MATRIX_HPP

#include <stdio.h>
#include <string.h>

#include <Arduino.h>

/**
 * @brief      Sums A and B and stores in C.
 *
 * @param      A     input  (n x m) matrix 
 * @param      B     input  (n x m) matrix
 * @param      C     output (n x m) matrix
 * @param[in]  n     rows of A = rows of B
 * @param[in]  m     cols of A = cols of B
 */
void sum(float* A, float* B, float* C, int n, int m);

/**
 * @brief      Subtracts A and B and stores in C.
 *
 * @param      A     input  (n x m) matrix 
 * @param      B     input  (n x m) matrix
 * @param      C     output (n x m) matrix
 * @param[in]  n     rows of A = rows of B
 * @param[in]  m     cols of A = cols of B
 */
void sub(float* A, float* B, float* C, int n, int m);

/**
 * @brief      Multiplies A and B and stores in C.
 *
 * @param      A     input  (n x m) matrix 
 * @param      B     input  (m x p) matrix
 * @param      C     output (n x p) matrix
 * @param[in]  n     rows of A
 * @param[in]  m     cols of A = rows of B 
 * @param[in]  p     cols of B
 */
void mul(float* A, float* B, float* C, int n, int m, int p);

/**
 * @brief      Multiplies A and B element-wise and stores in C.
 *
 * @param      A     input  (n x m) matrix 
 * @param      B     input  (n x m) matrix
 * @param      C     output (n x m) matrix
 * @param[in]  n     rows of A = rows of B
 * @param[in]  m     cols of A = cols of B
 */
void elemMul(float* A, float* B, float* C, int n, int m);

/**
 * @brief      Fills A with a given value.
 *
 * @param      A      (n x m) matrix
 * @param[in]  value  The value
 * @param[in]  n      rows of A
 * @param[in]  m      cols of A
 */
void fill(float *A, float value, int n, int m);

/**
 * @brief      (Deep) copies A to B.
 *
 * @param      A     input  (n x m) matrix 
 * @param      B     output (n x m) matrix
 * @param[in]  n     rows of A
 * @param[in]  m     cols of A
 */
void copy(float* A, float* B, int n, int m);

#endif
