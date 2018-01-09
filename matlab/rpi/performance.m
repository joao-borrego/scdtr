%% Configs

set(0,'DefaultTextFontname', 'CMU Serif')
set(0,'DefaultAxesFontName', 'CMU Serif')
args = {'interpreter','latex','FontSize',26};

%% a) Distributed Control

data_0 = csvread("data/a_0.csv");
data_1 = csvread("data/a_1.csv");

time_0 = data_0(:,1) / 1000.0; 
lux_0  = data_0(:,2);
out_0  = data_0(:,3) * 100.0;
ref_0  = data_0(:,4);
err_0  = data_0(:,5);
var_0  = data_0(:,6);

time_1 = data_1(:,1) / 1000.0; 
lux_1  = data_1(:,2);
out_1  = data_1(:,3) * 100.0;
ref_1  = data_1(:,4);
err_1  = data_1(:,5);
var_1  = data_1(:,6);

err_avg = (err_0 + err_1) / 2;
var_avg = (var_0 + var_1) / 2;

fig = figure('Name', "Distributed Control Node 0");
hold on;
plot(time_1, err_avg, 'LineWidth',1.3);
plot(time_1, var_avg, 'LineWidth',1.3);
hold off;
set(gca,'FontSize',24);
xlabel("Time [s]", args{:});
output_name = "figures/a_0.pdf";
fig.PaperPositionMode = 'auto';
fig_pos = fig.PaperPosition;
fig.PaperSize = [fig_pos(3) fig_pos(4)];
print(fig,'-dpdf','-r300',char(output_name));

fig = figure('Name', "Distributed Control Node 1");
hold on;
plot(time_1, lux_1, 'LineWidth',1.3);
plot(time_1, ref_1, 'LineWidth',1.3);
plot(time_1, out_1, 'LineWidth',1.3);
hold off;
set(gca,'FontSize',24);
xlabel("Time [s]", args{:});
output_name = "figures/a_1.pdf";
fig.PaperPositionMode = 'auto';
fig_pos = fig.PaperPosition;
fig.PaperSize = [fig_pos(3) fig_pos(4)];
print(fig,'-dpdf','-r300',char(output_name));

%% b) Non-distributed Control

data_0 = csvread("data/b_0.csv");
data_1 = csvread("data/b_1.csv");

time_0 = data_0(:,1) / 1000.0; 
lux_0  = data_0(:,2);
out_0  = data_0(:,3) * 100.0;
ref_0  = data_0(:,4);
err_0  = data_0(:,5);
var_0  = data_0(:,6);

time_1 = data_1(:,1) / 1000.0; 
lux_1  = data_1(:,2);
out_1  = data_1(:,3) * 100.0;
ref_1  = data_1(:,4);
err_1  = data_1(:,5);
var_1  = data_1(:,6);

fig = figure('Name', "Non-Distributed Control Node 0");
hold on;
plot(time_0, lux_0, 'LineWidth',1.3);
plot(time_0, ref_0, 'LineWidth',1.3);
plot(time_0, out_0, 'LineWidth',1.3);
hold off;
set(gca,'FontSize',24);
xlabel("Time [s]", args{:});
output_name = "figures/b_0.pdf";
fig.PaperPositionMode = 'auto';
fig_pos = fig.PaperPosition;
fig.PaperSize = [fig_pos(3) fig_pos(4)];
print(fig,'-dpdf','-r300',char(output_name));

fig = figure('Name', "Non-Distributed Control Node 1");
hold on;
plot(time_1, lux_1, 'LineWidth',1.3);
plot(time_1, ref_1, 'LineWidth',1.3);
plot(time_1, out_1, 'LineWidth',1.3);
hold off;
set(gca,'FontSize',24);
xlabel("Time [s]", args{:});
output_name = "figures/b_1.pdf";
fig.PaperPositionMode = 'auto';
fig_pos = fig.PaperPosition;
fig.PaperSize = [fig_pos(3) fig_pos(4)];
print(fig,'-dpdf','-r300',char(output_name));