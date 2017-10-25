%% Open a file and plot

a = csvread('data/fig_f_1_avg_10.csv');
x = linspace(1, 13000, 13000);
%plot(x, a);

b = mean(a);
plot(x, b);

%% Perform a linear regression

%p = polyfit(x, b, 1);
%hold on;
%y = p(1)*x + p(2);
%plot(x, y);