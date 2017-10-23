a = csvread('data/test.csv');
b = mean(a);
x = linspace(1, 256, 256);

p = polyfit(x, b, 1);

plot(x, b);
hold on;
y = p(1)*x + p(2);
plot(x, y);