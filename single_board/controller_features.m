%% Configs

set(0,'DefaultTextFontname', 'CMU Serif')
set(0,'DefaultAxesFontName', 'CMU Serif')

%% h) Feedforward comparison

files = ["fig_h_0",
         "fig_h_0_5",
         "fig_h_1"];
names = ["h.1) FF K_F = 0",
         "h.2) FF K_F = 0.5",
         "h.3) FF K_F = 1"];
args = {'interpreter','latex','FontSize',14};

for i = 1:3
    data = csvread(strcat("data/",files(i),".csv"));
    time = (data(:,5) / 1000.0) - 4.0;
    ref  = data(:,1);
    lx   = data(:,3);
    fig = figure('Name', names(i));
    hold on;
    plot(time, ref);
    plot(time, lx);
    hold off;
    xlabel("Time [s]", args{:});
    ylabel("Illuminance [lx]", args{:});
    legend({'ref(t)','y(t)'}, 'Location','southeast',args{:});
    output_name = sprintf("figures/%s.pdf",files(i));
    fig.PaperPositionMode = 'auto';
    fig_pos = fig.PaperPosition;
    fig.PaperSize = [fig_pos(3) fig_pos(4)];
    print(fig,'-dpdf','-r300',char(output_name));
end

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