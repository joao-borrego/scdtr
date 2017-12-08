/**
 * @file consensus.hpp
 */

#include "matrix.hpp"
#include "constants.hpp"

/** */
const float ITERATIONS = 50;
/** */
const float RHO = 0.01;
/** */
const float Q[N] = {0.0, 0.0};
/** */
const float C[N] = {0.0, 0.0};

/* Functions */

float costFunction(float* d_i);

int solve(size_t id, float* L, float* K_i, float o);