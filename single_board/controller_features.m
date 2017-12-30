%% h) Feedforward comparison

data = csvread('data/fig_h_0.csv');
time = (data(:,5) / 1000.0) - 4.0;
ref  = data(:,1);
lx   = data(:,3);
figure('Name','h.1) FF K_F = 0');
hold on;
plot(time, ref);
plot(time, lx);
hold off;

data = csvread('data/fig_h_1.csv');
time = (data(:,5) / 1000.0) - 4.0;
ref  = data(:,1);
lx   = data(:,3);
figure('Name','h.2) FF K_F = 1');
hold on;
plot(time, ref);
plot(time, lx);
hold off;

data = csvread('data/fig_h_0_5.csv');
time = (data(:,5) / 1000.0) - 4.0;
ref  = data(:,1);
lx   = data(:,3);
figure('Name','h.3) FF K_F = 0.5');
hold on;
plot(time, ref);
plot(time, lx);
hold off;

%% i) FF + Delayed PI comparison

data = csvread('data/fig_i_1.csv');
time = (data(:,5) / 1000.0) - 4.0;
ref  = data(:,1);
lx   = data(:,3);
figure('Name','i.1) Delay = 1 Sample, FF K_F = 0.5');
hold on;
plot(time, ref);
plot(time, lx);
hold off;

data = csvread('data/fig_i_2.csv');
time = (data(:,5) / 1000.0) - 4.0;
ref  = data(:,1);
lx   = data(:,3);
figure('Name','i.1) Delay = 2 Samples, FF K_F = 0.5');
hold on;
plot(time, ref);
plot(time, lx);
hold off;

data = csvread('data/fig_i_3.csv');
time = (data(:,5) / 1000.0) - 4.0;
ref  = data(:,1);
lx   = data(:,3);
figure('Name','i.1) Delay = 3 Samples, FF K_F = 0.5');
hold on;
plot(time, ref);
plot(time, lx);
hold off;

%% j) Anti-windup comparison

data = csvread('data/fig_j_1.csv');
time = (data(:,5) / 1000.0) - 4.0;
ref  = data(:,1);
lx   = data(:,3);
figure('Name','j.1) Anti-windup off');
hold on;
plot(time, ref);
plot(time, lx);
hold off;

data = csvread('data/fig_j_2.csv');
time = (data(:,5) / 1000.0) - 4.0;
ref  = data(:,1);
lx   = data(:,3);
figure('Name','j.2) Simple Anti-windup');
hold on;
plot(time, ref);
plot(time, lx);
hold off;

data = csvread('data/fig_j_3.csv');
time = (data(:,5) / 1000.0) - 4.0;
ref  = data(:,1);
lx   = data(:,3);
figure('Name','j.3) Complex Anti-windup K_SAT = 0.5');
hold on;
plot(time, ref);
plot(time, lx);
hold off;

%% k) Error deadzone comparison

data = csvread('data/fig_k_1.csv');
time = (data(:,5) / 1000.0) - 4.0;
ref  = data(:,1);
out   = data(:,2) * 100.0;
figure('Name','k.1) Error deadzone off');
hold on;
yyaxis left
ylim([25, 55])
plot(time, ref);
yyaxis right
ylim([40, 85])
plot(time, out);
hold off;

data = csvread('data/fig_k_2.csv');
time = (data(:,5) / 1000.0) - 4.0;
ref  = data(:,1);
out   = data(:,2) * 100.0;
figure('Name','k.2) Error deadzone [-0.5,0.5]');
hold on;
yyaxis left
ylim([25, 55])
plot(time, ref);
yyaxis right
ylim([40, 85])
plot(time, out);
hold off;

data = csvread('data/fig_k_3.csv');
time = (data(:,5) / 1000.0) - 4.0;
ref  = data(:,1);
out   = data(:,2) * 100.0;
figure('Name','k.3) Error deadzone [-1.6,1.6]');
hold on;
yyaxis left
ylim([25, 55])
plot(time, ref);
yyaxis right
ylim([40, 85])
plot(time, out);
hold off;