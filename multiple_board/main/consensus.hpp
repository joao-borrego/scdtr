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
const float C[N] = {0.0, 0.0};

/* Functions */

float costFunction(float* d_i);

int solve(size_t id, float* L, float* K_i, float o);

}
