% Read impulse response to an array
y = csvread('data/fig_g_1.csv');

%% Open PID Tuner app
% PID Tuner > Plant > Identify New Plant
% Plant Identification > Get I/O Data > Step Response
%   Output signal   y
%   Amplitude A     128
%   Offset          0
%   Onset Lag       0.15 [s]
%   Start Time      0
%   Sample Time     0.0005 [s]
%   Units           seconds
% Plant Identification > Auto-estimate and Apply
% PID Tuner > Export > Plant1

%% Obtain continuous transfer function of the exported plant
systf = tf(Plant1);
% Discretize system using Tustin method and obtain transfer function w/ 
% controller sample time of 0.030 seconds
sysd = c2d(systf, 0.03, 'tustin');

%% Open PID Tuner app again
%   PID Tuner > Import > sysd
%   PID Tuner > Type > PI and Parallel (to decouple k_p and k_i)
%   Tune parameters to obtain (arbitrary but decent and memorable values)
%       Response time  0.6 seconds
%       Robustness     0.85
%   Resulted in 
%       Rise Time      0.63 seconds
%       Settling time  1.14 seconds
%       Overshoot      0 %
%       Phase margin   87.7 degrees

k_p = 2.051
k_i = 136.7362