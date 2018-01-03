/**
 * @file    multiple_board/main/consensus.cpp
 *
 * @brief   Consensus algorithm implementation
 *
 * Consensus distributed algorithm implementation for obtaining a solution
 * to a global optimisation problem.
 *
 * @author  João Borrego
 * @author  António Almeida
 */

#include "consensus.hpp"

namespace Consensus{

/* Global variables */

/** Node identifier */
int id;
/** Lock progression */
volatile bool ready = false;

/** Received best solution */
volatile float d_tmp[N]     = {0.0};
/** Sum of neighbours' solutions */
volatile float d_copies[N]  = {0.0};
/** Current sender id */
uint8_t sender;
/** Current receiver id */
uint8_t receiver;

/* Functions */

void debugPrint(float *d, int size, int it, char* description)
{
    /*
    Serial.print("[Consensus] ");
    Serial.print(it);
    Serial.print(" - ");
    Serial.print(description);
    Serial.print(" [ ");
    for (int j = 0; j < size; j++) { Serial.print(d[j]); Serial.print(" "); }
    Serial.println("]");
    */
}

float costFunction(
    float *d_i,
    float *d_i_avg,
    float q_i,
    float c_i,
    float *y_i,
    float rho)
{
    // Auxiliary variables;
    float a    = 0.0;
    float b    = 0.0;
    float c    = 0.0;
    float d    = 0.0;
    float e    = 0.0;
    float A[N] = {0.0};
    float cost = 0.0;

    // a = 1/2 d_i' * Q_i * d_i
    mul(d_i, d_i, &a, 1, N, 1);
    a = q_i / 2.0 * a;
    // b = d_i' * c_i
    b = d_i[id] * c_i;
    // c = d_i' * y_i
    mul(d_i, y_i, &c, 1, N, 1);
    // d = d_i_avg' * y_i
    mul(d_i_avg, y_i, &d, 1, N, 1);
    // A = d_i - d_i_avg
    sub(d_i, d_i_avg, A, N, 1);
    // e = rho/2 * (d_i - d_i_avg)' * (d_i - d_i_avg)
    mul(A, A, &e, 1, N, 1);
    e = rho / 2.0 * e;

    cost = a + b + c - d + e;
    return cost;
}

bool checkConstraints(
    float *d_i,
    float *K_i,
    float L,
    float o,
    bool check_linear)
{
    for (int j = 0; j < N; j++){
        if (d_i[j] > MAX_DC || d_i[j] < 0.0){
            return false;
        }
    }

    if (check_linear){
        float prod = 0.0;
        float u = L - o;
        mul(K_i, d_i, &prod, 1, N, 1);
        if (prod < u){
            return false;
        }
    }

    return true;
}

void updateBest(
    float *d_i,
    float *K_i,
    float L,
    float o,
    bool check_linear,
    float *d_i_best,
    float *cost_best,
    float *d_i_avg,
    float q_i,
    float c_i,
    float *y_i,
    float rho)
{
    // Check if solution is feasible
    bool feasible = checkConstraints(d_i, K_i, L, o, check_linear);
    if (!feasible){
        return;
    }
    // Update minimum if cost is lower
    float cost = costFunction(d_i, d_i_avg, q_i, c_i, y_i, rho);
    if (cost < *cost_best){
        *cost_best = cost;
        copy(d_i, d_i_best, N, 1);
    }
}

void getCopies(float *d_i_best)
{
    // Setup
    fill(d_copies, 0.0, N, 1);
    Wire.onReceive(Consensus::onReceive);

    for (sender = 0; sender < N; sender++){
        if (id == sender){
            for (receiver = 0; receiver < N; receiver++){
                if (sender != receiver){
                    while(!ready){
                        Communication::sendConsensus(receiver, false, d_i_best);
                        delay(50);
                    }
                    ready = false;
                }
            }
        } else {
            while (!ready) { }
            ready = false;
            Communication::sendPacket(sender, ACK);
        }
    }

    Wire.onReceive(Communication::nop);
}

void onReceive(int bytes)
{
    byte p_id, p_type;
    size_t p_size = bytes;

    byte packet[MAX_SIZE];
    Communication::readPacket(&p_id, &p_type, p_size, packet);

    if (id == sender)
    {
        if (p_id == receiver && p_type == ACK){
            ready = true;
        }
    }
    else
    {
        if (p_id == sender && p_type == CON){

            // Read neighbour solution and add to sum
            Communication::readConsensus(packet + HEADER_SIZE, d_tmp);
            for (int j = 0; j < N; j++){
                d_copies[j] += d_tmp[j];
            }
            ready = true;
        }
    }
}

float solve(size_t id_, float L, float* K_i, float o)
{
    // Setup
    id = id_;

    // Duty cycle for current iteration
    float d_i[N]        = {0.0};
    // Aux variable for intermediate calculations
    // Standard matrix multiplications cannot have any common operand and result matrix
    // since the original matrix would be overwritten and its data lost
    float d_i_aux[N]    = {0.0};
    // Best duty cycle for current iteration
    float d_i_best[N]   = {0.0};
    // Averaged best solution
    float d_i_avg[N]    = {0.0};
    // New lux reference
    float output        = 0.0;

    // Unconstrained solution
    float d_i_0[N]      = {0.0};

    // Augmented Lagrangian method penalty parameter
    float rho           = RHO;
    // All other elements of Q are 0
    float q_i           = Q[id];
    // All other elements of C are 0
    float c_i           = C[id];
    // Cost of best solution for current iterations
    float cost_best     = INFINITY;
    // Lagrange multipliers
    float y_i[N]        = {0.0};
    // Diagonal of full P_i matrix
    float P_i[N]        = {0.0};
    // Diagonal of full R_i matrix
    float R_i[N]        = {0.0};
    // Diagonal of full Z_i matrix
    float Z_i[N]        = {0.0};
    // K_i but K_ij, j=id is removed, i.e. set to 0
    float K_i_r[N]      = {0.0};

    float n_i           = 0.0;
    float g_i           = 0.0;
    float v_i           = 0.0;

    // Auxiliary var for constraint 1
    float aux_1         = 0.0;
    // Auxiliary var for constraints 4
    float aux_2         = 0.0;
    float aux_3         = 0.0;
    float aux_4         = 0.0;
    // Auxiliary var for constraint 5
    float aux_5         = 0.0;

    // Auxiliary vector for constraints 4 and 5 (K_i * v_i / rho)
    float aux_m_1[N]    = {0.0};
    // Auxiliary vector for constraint 5
    float aux_m_2[N]    = {0.0};
    // (‖k_r‖_2) ^ 2
    float k_r_squared_two_norm = 0.0;

    for (int j = 0; j < N; j++){
        R_i[j] = (j == id)? (rho + q_i) : rho;
    }
    for (int j = 0; j < N; j++){
        P_i[j] = 1.0 / (R_i[j]);
    }
    for (int j = 0; j < N; j++){
        K_i_r[j] = (j == id)? 0 : K_i[j];
    }

    for (int j = 0; j < N; j++){
        n_i += (K_i[j] * K_i[j]) * P_i[j];
    }
    g_i = (rho + q_i) / (n_i * (rho + q_i) - (K_i[id] * K_i[id]));
    for (int j = 0; j < N; j++){
        k_r_squared_two_norm += (K_i_r[j] * K_i_r[j]);
    }

    debugPrint(&L, 1, -1, "L");
    debugPrint(K_i, N, -1, "K_i");
    debugPrint(P_i, N, -1, "P_i");
    debugPrint(&n_i, 1, -1, "n_i");
    debugPrint(&g_i, 1, -1, "g_i");
    debugPrint(&k_r_squared_two_norm, 1, -1, "‖k_r‖_2 ^ 2");

    for (int it = 0; it < ITERATIONS; it++){

        cost_best = INFINITY;

        for (int j = 0; j < N; j++){
            Z_i[j] = rho * d_i_avg[j] - y_i[j] - ((j == id)? c_i : 0);
        }
        debugPrint(Z_i, N, it, "Z_i");

        // Unconstrained
        elemMul(P_i, Z_i, d_i_0, N, 1);

        debugPrint(d_i_0, N, it, "d_0");

        updateBest(d_i_0, K_i, L, o, true, d_i_best, &cost_best,
            d_i_avg, q_i, c_i, y_i, rho);

        // Constrained linear boundary: u_1 = o_i - L_i
        aux_1 = o - L;
        for (int j = 0; j < N; j++){
            aux_1 += (K_i[j] * Z_i[j] * P_i[j]);
        }
        aux_1 = aux_1 / n_i;

        elemMul(K_i, P_i, d_i_aux, N, 1);
        mul(d_i_aux, &aux_1, d_i, N, 1, 1);
        sub(d_i_0, d_i, d_i, N, 1);

        debugPrint(d_i, N, it, "d_1");

        updateBest(d_i, K_i, L, o, false, d_i_best, &cost_best,
            d_i_avg, q_i, c_i, y_i, rho);

        // Constrained 0 boundary: u_2 = 0
        for (int j = 0; j < N; j++){
            d_i[j] = (j == id)? 0.0 : d_i_0[j];
        }

        debugPrint(d_i, N, it, "d_2");

        updateBest(d_i, K_i, L, o, true, d_i_best, &cost_best,
            d_i_avg, q_i, c_i, y_i, rho);

        // Constrained 100 boundary: u_3 = 100 (%)
        for (int j = 0; j < N; j++){
            d_i[j] = (j == id)? MAX_DC : d_i_0[j];
        }

        debugPrint(d_i, N, it, "d_3");

        updateBest(d_i, K_i, L, o, true, d_i_best, &cost_best,
            d_i_avg, q_i, c_i, y_i, rho);

        // d_4 Constrained to linear and 0 boundaries
        aux_2 = L - o;
        mul(K_i, &aux_2, d_i_aux, N, 1, 1);

        mul(Z_i, K_i_r, &v_i, 1, N, 1);
        aux_3 = v_i / rho;
        mul(K_i, &aux_3, aux_m_1, N, 1, 1);

        sub(d_i_aux, aux_m_1, d_i_aux, N, 1);
        d_i_aux[id] = (k_r_squared_two_norm * Z_i[id]) / (rho + q_i); // * -1.0 (?)

        aux_4 = g_i / rho;
        mul(d_i_aux, &aux_4, d_i, N, 1, 1);
        //debugPrint(d_i, N, it, "d_4_1");
        sum(d_i_0, d_i, d_i, N, 1);

        debugPrint(d_i, N, it, "d_4");

        updateBest(d_i, K_i, L, o, false, d_i_best, &cost_best,
            d_i_avg, q_i, c_i, y_i, rho);

        // d_5 Constrained to linear and 100 boundaries
        mul(K_i, &aux_2, d_i, N, 1, 1);

        aux_5 = MAX_DC  * K_i[id];
        mul(K_i, &aux_5, aux_m_2, N, 1, 1);

        sub(d_i, aux_m_2, d_i, N, 1);
        sub(d_i, aux_m_1, d_i, N, 1);
        d_i[id] = (MAX_DC * k_r_squared_two_norm)
            - (k_r_squared_two_norm * Z_i[id] / (rho + q_i));

        mul(d_i, &aux_4, d_i_aux, N, 1, 1);
        sum(d_i_0, d_i_aux, d_i, N, 1);

        debugPrint(d_i, N, it, "d_5");

        updateBest(d_i, K_i, L, o, false, d_i_best, &cost_best,
            d_i_avg, q_i, c_i, y_i, rho);

        debugPrint(d_i_best, N, it, "d_best");

        getCopies(d_i_best);

        copy(d_copies, d_i_avg, N, 1);
        for (int j = 0; j < N; j++){
            d_i_avg[j] += d_i_best[j];
            d_i_avg[j] = d_i_avg[j] / N;
        }
        debugPrint(d_i_avg, N, it, "d_i_avg");

        // d_i and d_i_aux are bith used as aux vectors here
        sub(d_i_best, d_i_avg, d_i_aux, N, 1);
        mul(d_i_aux, &rho, d_i, N, 1, 1);
        sum(y_i, d_i, y_i, N, 1);

        debugPrint(y_i, N, it, "y_i");
        debugPrint(&cost_best, 1, it, "cost_best");
    }

    mul(K_i, d_i_best, &output, 1, N, 1);
    return output;
}

}