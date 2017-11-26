%% d) Voltage in ldr as a function of led pwm signal

y = csvread('data/fig_d_1.csv');
x = linspace(0, 255, 256);
figure('Name','d.1) Sweep v_ldr[V] - T = 100 ms, 1 sample');
plot(x, y);

y = csvread('data/fig_d_2.csv');
x = linspace(0, 25, 26);
figure('Name','d.2) Sweep v_ldr[V] - T = 100 ms, 1 sample');
plot(x, y);

y = csvread('data/fig_d_3.csv');
x = linspace(0, 12999, 13000);
figure('Name','d.3) Sweep v_ldr[V] - T = 0.2 ms, 500 samples');
plot(x, y);

y = csvread('data/fig_d_3_avg_5.csv');
y_mean = mean(y);
figure('Name','d.3 avg) Sweep v_ldr[V] - T = 0.2 ms, 500 samples, avg 5 runs');
plot(x, y_mean);

%% e) Steady-state voltage in ldr raw ADC input as a function of led pwm

y = csvread('data/fig_e.csv');
x = linspace(0, 255, 256);
figure('Name','e) Sweep in_ldr[raw] - T = 50 ms, 1 sample');
plot(x, y);

%% f) Previous figures in LUX units

y = csvread('data/fig_f_1.csv');
x = linspace(0, 12999, 13000);
figure('Name','f.1) Sweep in_ldr[LUX] - T = 0.2 ms, 500 samples');
plot(x, y);

y = csvread('data/fig_f_1_avg_10.csv');
y_mean = mean(y);
figure('Name','f.1 avg) Sweep in_ldr[LUX] - T = 0.2 ms, 500 samples, avg 10 runs');
plot(x, y_mean);

y = csvread('data/fig_f_2.csv');
x = linspace(0, 255, 256);
figure('Name','f.2) Sweep in_ldr[LUX] - T = 50 ms, 1 sample');
plot(x, y);

y = csvread('data/fig_f_2_avg_10.csv');
y_mean = mean(y);
figure('Name','f.2 avg) Sweep in_ldr[LUX] - T = 50 ms, 1 sample, avg 10 runs');
plot(x, y_mean);

% Perform linear regression on the 10-run averaged figure in order to
% obtain a linear approximation y = m*x + b needed for feedforward control
% (as opposed to using a lookup table)
[r, m, b] = regression(x, y_mean)
hold on;
y = m * x + b;
plot(x, y);

% m = 0.4330
% b = -4.6204

%% g) Step response

y = csvread('data/fig_g_1.csv');
x = linspace(0, 599, 600);
figure('Name','g.1) Sweep v_ldr[V] - T = 0.5 ms, A = 128, 300 samples');
plot(x, y);

% Calculate time constant, using the 10-run averaged figure
y = csvread('data/fig_g_1_avg_10.csv');
t = zeros(10,1);
for i = 1:10
    single_y = y(i,:);
    y_f = single_y(end);
    idx = max(find(abs(single_y-y_f)>=0.37*y_f));
    t(i) = (idx - 300) * 0.5; 
end
t_avg = mean(t) % ms ± 0.5 ms

y = csvread('data/fig_g_2.csv');
figure('Name','g.2) Sweep in_ldr[LUX] - T = 0.5 ms, A = 128, 300 samples');
plot(x, y);
