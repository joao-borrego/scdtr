### CSV Data files description

#### d) Voltage in ldr as a function of led pwm signal

| File          | Command                               | Parameters                          |
|---------------|---------------------------------------|-------------------------------------|
|`fig_d_1.csv`  | `sweep(0, 255, 1, 100, 1, VOLT)`      | T = 100 ms step = 1  samples = 1    |
|`fig_d_2.csv`  | `sweep(0, 255, 1, 100, 1, VOLT)`      | T = 100 ms step = 10 samples = 1    |
|`fig_d_3.csv`  | `sweep(0, 255, 10, 0.2, 500, VOLT)`   | T = 0.2 ms step = 10 samples = 500  |

#### e) Steady-state voltage in ldr raw ADC input as a funciton of led pwm signal

| File          | Command                               | Parameters                          |
|---------------|---------------------------------------|-------------------------------------|
|`fig_e.csv`    | `sweep(0, 255, 10, 50, 1, RAW)`       | T = 50 ms  step = 1                 |

#### f) Previous figures in LUX units

| File          | Command                               | Parameters                          |
|---------------|---------------------------------------|-------------------------------------|
|`fig_f_1.csv`  | `sweep(0, 255, 10, 0.2, 500, LUX)`    | T = 0.2 ms step = 10 samples = 500  |
|`fig_e.csv`    | `sweep(0, 255, 10, 50, 1, LUX)`       | T = 50 ms  step = 1                 |

#### g) Step response for calculating time constant

| File          | Command                               | Parameters                          |
|---------------|---------------------------------------|-------------------------------------|
|`fig_g_1.csv`  | `sweep(0, 128, 128, 0.1, 1000, VOLT)` | T = 0.1 ms A = 128 samples = 1000   |