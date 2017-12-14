/**
 * @file consensus.cpp
 *
 * @brief
 *
 * For algorithm details check the docs
 */

#include "consensus.hpp"

/* Global variables */

/** Current id */
volatile int cur            = 0;
/** Received best solution */
volatile float d_tmp[N]     = {0.0};
/** Sum of neighbours' solutions */
volatile float d_copies[N]  = {0.0};

volatile bool ready = false;

namespace Consensus{

/* Functions */

void debugPrint(float *d, int size, int it, char* description)
{
    Serial.print("[Consensus] ");
    Serial.print(it);
    Serial.print(" - ");
    Serial.print(description);
    Serial.print(" [ ");
    for (int j = 0; j < size; j++) { Serial.print(d[j]); Serial.print(" "); }
    Serial.println("]");
}

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
    float prod = 0.0;
    float u = L - o;

    for (int j = 0; j < N; j++){
        if (d_i[j] > 100.0 || d_i[j] < 0.0) return false;
    }
    mul(K_i, d_i, &prod, 1, N, 1);
    if (prod < u) return false;

    return true;
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

void getCopies(uint8_t id, float *d_i_best)
{
    // Setup
    cur = 0;
    fill(d_copies, 0.0, N, 1);
    Wire.onRequest(Communication::onRequest);
    Wire.onReceive(Consensus::onReceive);
    Communication::barrier(id);

    for (int j = 0; j < N; j++){

        if (j == id){
            for (int k = 0; k < N; k++){
                if (k != id){
                    Communication::sendConsensus(k, false, d_i_best);
                }
            }
        } else {
            while (!ready){}
            ready = false;
        }
        cur++;
    }

    Wire.onRequest(Communication::nop);
    Wire.onReceive(Communication::nop);
}

void onReceive(int bytes)
{

    byte msg[MAX_SIZE] = {0};
    size_t size = Communication::readToBuffer(msg);

    uint8_t id = msg[ID];
    uint8_t type = msg[TYPE];
    if (id == cur && type == CON){
        Communication::readConsensus(msg + HEADER_SIZE, d_tmp);

        Serial.print("[Msg] ");

        for (int j = 0; j < N; j++){
            d_copies[j] += d_tmp[j];

            Serial.print(d_tmp[j]);
            Serial.print(" ");
        }
        Serial.println();
        ready = true;
    }
}

int solve(size_t id, float L, float* K_i, float o)
{

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

    // Unconstrained solution
    float d_i_0[N]      = {0.0};

    float rho           = RHO;
    // All other elements of Q are 0
    float q_i           = Q[id];
    // All other elements of C are 0
    float c_i           = C[id];

    float cost_best     = INFINITY;

    float y_i[N]        = {0.0};
    // Diagonal of full P_i matrix
    float P_i[N]        = {0.0};
    // Diagonal of full R_i matrix
    float R_i[N]        = {0.0};
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

    debugPrint(K_i, N, -1, "K_i");
    debugPrint(P_i, N, -1, "P_i");
    debugPrint(&n_i, 1, -1, "n_i");
    debugPrint(&g_i, 1, -1, "g_i");
    debugPrint(&k_r_squared_two_norm, 1, -1, "‖k_r‖_2 ^ 2");

    for (int it = 0; it < ITERATIONS; it++){

        for (int j = 0; j < N; j++){
            Z_i[j] = rho * d_i_avg[j] - y_i[j] - ((j == id)? c_i : 0);
        }
        debugPrint(Z_i, N, it, "Z_i");

        // Unconstrained
        elemMul(P_i, Z_i, d_i_0, N, 1);

        debugPrint(d_i_0, N, it, "d_0");

        updateBest(d_i_0, K_i, L, o, d_i_best, &cost_best, R_i, Z_i);

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

        updateBest(d_i, K_i, L, o, d_i_best, &cost_best, R_i, Z_i);

        // Constrained 0 boundary: u_2 = 0
        for (int j = 0; j < N; j++){
            d_i[j] = (j == id)? 0.0 : d_i_0[j];
        }

        debugPrint(d_i, N, it, "d_2");

        updateBest(d_i, K_i, L, o, d_i_best, &cost_best, R_i, Z_i);

        // Constrained 100 boundary: u_3 = 100 (%)
        for (int j = 0; j < N; j++){
            d_i[j] = (j == id)? 100.0 : d_i_0[j];
        }

        debugPrint(d_i, N, it, "d_3");

        updateBest(d_i, K_i, L, o, d_i_best, &cost_best, R_i, Z_i);

        // d_4 Constrained to linear and 0 boundaries
        aux_2 = L - o;
        mul(K_i, &aux_2, d_i_aux, N, 1, 1);

        mul(Z_i, K_i_r, &v_i, 1, N, 1);
        aux_3 = v_i / rho;
        mul(K_i, &aux_3, aux_m_1, N, 1, 1);

        sub(d_i_aux, aux_m_1, d_i_aux, N, 1);
        d_i_aux[id] = (k_r_squared_two_norm * Z_i[id]) / (rho + q_i);

        aux_4 = g_i / rho;
        mul(d_i_aux, &aux_4, d_i, N, 1, 1);
        //debugPrint(d_i, N, it, "d_4_1");
        sum(d_i_0, d_i, d_i, N, 1);

        debugPrint(d_i, N, it, "d_4");

        updateBest(d_i, K_i, L, o, d_i_best, &cost_best, R_i, Z_i);

        // d_5 Constrained to linear and 100 boundaries
        mul(K_i, &aux_2, d_i, N, 1, 1);

        aux_5 = 100.0  * K_i[id];
        mul(K_i, &aux_5, aux_m_2, N, 1, 1);

        sub(d_i, aux_m_2, d_i, N, 1);
        sub(d_i, aux_m_1, d_i, N, 1);
        d_i[id] = (100.0 * k_r_squared_two_norm)
            - (k_r_squared_two_norm * Z_i[id] / (rho + q_i));

        mul(d_i, &aux_4, d_i_aux, N, 1, 1);
        sum(d_i_0, d_i_aux, d_i, N, 1);

        debugPrint(d_i, N, it, "d_5");

        updateBest(d_i, K_i, L, o, d_i_best, &cost_best, R_i, Z_i);

        debugPrint(d_i_best, N, it, "d_best");

        getCopies(id, d_i_best);

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
    }

    return round(d_i_best[id]);
}

}