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

    // TODO
    float costFunction(
        uint8_t id,
        float *d_i,
        float *d_i_avg,
        float q_i,
        float c_i,
        float *y_i, 
        float rho);

    // TODO
    bool checkConstraints(
        float *d_i,
        float *K_i,
        float L,
        float o,
        bool check_linear);

    // TODO
    void updateBest(
        uint8_t id,
        float *d_i,
        float *K_i,
        float L,
        float o,
        bool check_linear, // Check linear constraint
        float *d_i_best,
        float *cost_best,
        float *d_i_avg,
        float q_i,
        float c_i,
        float *y_i,
        float rho);

    // TODO
    void getAverageSolution(uint8_t id, float *d_i_best);

    // TODO
    void onReceive(int bytes);

    // TODO
    float solve(size_t id, float L, float* K_i, float o);

}
