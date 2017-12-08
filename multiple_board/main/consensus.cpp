/**
 * @file consensus.cpp
 */

#include "consensus.hpp"

float costFunction(float *d_i, float *R_i, float *Z_i)
{
    // Auxiliary variables
    float A[N]      = {0.0};
    // cost = 1/2 * cost_a - cost_b 
    float cost_a    = 0.0;
    float cost_b    = 0.0;

    // aux A = R_i * d_i 
    elemMul(R_i, d_i, A, N, 1);
    // cost_a = d_i' * R_i * d_i = d_i' * A
    mul(d_i, A, &cost_a, 1, N, 1);
    // cost b = d_i' * Z_i
    mul(d_i, Z_i, &cost_b, 1, N, 1);

    return (0.5 * cost_a - cost_b);
}

bool checkConstraints(float *d_i, float *K_i, float L, float o)
{
    return false;
}

void updateBest(
    float *d_i,
    float *K_i,
    float L,
    float o,
    float *d_i_best,
    float *cost_best,
    float *R_i,
    float *Z_i)
{
    // Check if solution is feasable
    bool feasable = checkConstraints(d_i, K_i, L, o);
    if (!feasable) return;
    // Update minimum if cost is lower
    float cost = costFunction(d_i, R_i, Z_i);
    if (cost < *cost_best){
        *cost_best = cost;
        copy(d_i, d_i_best, N, 1);
    }
}

int solve(size_t id, float* L, float* K_i, float o)
{

    // Duty cycle for current iteration
    float d_i[N]        = {0.0};
    // Best duty cycle for current iteration
    float d_i_best[N]   = {0.0};
    // Last iteration duty cycle result
    float d_i_prev[N]   = {0.0};

    float rho           = RHO;
    float q_i           = Q[id];
    float c_i           = C[id];

    float cost_best     = INFINITY;

    float y_i[N]        = {0.0};
    float P_i[N]        = {0.0};
    float R_i[N]        = {0.0};
    float Z_i[N]        = {0.0};

    for (int j = 0; j < N; j++){
        R_i[j] = (j == id)? (rho + q_i) : rho;
    }
    for (int j = 0; j < N; j++){
        P_i[j] = 1.0 / (R_i[j]);
    }

    for (int it = 0; it < ITERATIONS; it++){

        for (int j = 0; j < N; j++){
            Z_i[j] = rho * d_i[j] - y_i[j] - ((j == id)? c_i : 0);
        }

        // d0 Unconstrained solution
        elemMul(P_i, Z_i, d_i, N, 1);
        updateBest(d_i, K_i, L[id], o, d_i_best, &cost_best, R_i, Z_i);

        // TODO
    }

    return d_i_best[id];
}
