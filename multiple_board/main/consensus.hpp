/**
 * @file consensus.hpp
 */

#include "matrix.hpp"
#include "constants.hpp"
#include "communication.hpp"

namespace Consensus{

/** Total number of iterations */
const float ITERATIONS = 50;
/** Rho */
const float RHO = 0.01;
/** TODO */
const float Q[N] = {0.0, 0.0};
/** TODO */
const float C[N] = {1.0, 1.0};

/* Functions */

float costFunction(float* d_i);

bool checkConstraints(float *d_i, float *K_i, float L, float o);

void updateBest(
    float *d_i,
    float *K_i,
    float L,
    float o,
    float *d_i_best,
    float *cost_best,
    float *R_i,
    float *Z_i);

void getAverageSolution(uint8_t id, float *d_i_best);

void onReceive(int bytes);

int solve(size_t id, float L, float* K_i, float o);

}
