%% Configs

set(0,'DefaultTextFontname', 'CMU Serif')
set(0,'DefaultAxesFontName', 'CMU Serif')
args = {'interpreter','latex','FontSize',20};
args_1 = {'interpreter','latex','FontSize',18};

%% a) Distributed Control

data_0 = csvread("data/a_0.csv");
data_1 = csvread("data/a_1.csv");

time_0 = data_0(:,8) / 1000.0; 
lux_0  = data_0(:,1);
out_0  = data_0(:,2) * 100.0;
ref_0  = data_0(:,5);

d_time_0 = data_0(:,8) / 1000.0; 

time_1 = data_1(:,8) / 1000.0; 
lux_1  = data_1(:,1);
out_1  = data_1(:,2) * 100.0;
ref_1  = data_1(:,5);

e_0         = zeros(min(size(lux_0,1), size(lux_1,1)),1);
e_1         = zeros(min(size(lux_0,1), size(lux_1,1)),1);
d_e_t       = zeros(min(size(lux_0,1), size(lux_1,1)),1);
c_err_0     = zeros(min(size(lux_0,1), size(lux_1,1)),1);
c_err_1     = zeros(min(size(lux_0,1), size(lux_1,1)),1);
d_c_err_t   = zeros(min(size(lux_0,1), size(lux_1,1)),1);
c_var_0     = zeros(min(size(lux_0,1), size(lux_1,1)),1);
c_var_1     = zeros(min(size(lux_0,1), size(lux_1,1)),1);
d_c_var_t   = zeros(min(size(lux_0,1), size(lux_1,1)),1);

sum = 0;
for j = 2:size(e_0,1)
    sum = sum + out_0(j-1)*(time_0(j) - time_0(j-1))/1000;
    e_0(j) = sum;
end;
sum = 0;
for j = 2:size(e_1,1)
    sum = sum + out_1(j-1)*(time_1(j) - time_0(j-1))/1000;
    e_1(j) = sum;
end;
d_e_t = e_0 + e_1; 

sum = 0;
for j = 1:size(c_err_0,1)
    sum = sum + max(ref_0(j) - lux_0(j), 0);
    c_err_0(j) = 1/j * sum;
end;
sum = 0;
for j = 1:size(c_err_1,1)
    sum = sum + max(ref_1(j) - lux_1(j), 0);
    c_err_1(j) = 1/j * sum;
end;
d_c_err_t = c_err_0 + c_err_1;

sum = 0;
for j = 3:size(c_var_0,1)
    sum = sum + abs(lux_0(j) - 2*lux_0(j-1) + lux_0(j-2));
    c_var_0(j) = 1/j * sum / (0.100 * 0.100);
end;
sum = 0;
for j = 3:size(c_var_1,1)
    sum = sum + abs(lux_1(j) - 2*lux_1(j-1) + lux_1(j-2));
    c_var_1(j) = 1/j * sum / (0.100 * 0.100);
end;
d_c_var_t = c_var_0 + c_var_1;

fig = figure('Name', "Distributed Control Node 0");
hold on;
yyaxis left
ylim([0,100]);
ylabel("Illuminance [lx]", args{:});
plot(time_0, lux_0, 'LineWidth',1);
plot(time_0, ref_0, '-','Color', [0.9290 0.6940 0.1250], 'LineWidth',2);
yyaxis right
ylim([0,100]);
ylabel("Duty cycle [\%]", args{:});
plot(time_0, out_0, 'LineWidth',1);
hold off;
xlim([0,100]);
set(gca,'FontSize',18);
xlabel("Time [s]", args{:});
legend({'y(t)','r(t)','out(t)'}, 'Location','south','Orientation', 'horizontal', args_1{:});
output_name = "figures/fig_a_0.pdf";
fig.PaperPositionMode = 'auto';
fig_pos = fig.PaperPosition;
fig.PaperSize = [fig_pos(3) fig_pos(4)];
print(fig,'-dpdf','-r300',char(output_name));

fig = figure('Name', "Distributed Control Node 1");
hold on;
yyaxis left
ylim([0,100]);
ylabel("Illuminance [lx]", args{:});
plot(time_1, lux_1, 'LineWidth',1);
plot(time_1, ref_1, '-','Color', [0.9290 0.6940 0.1250], 'LineWidth',2);
yyaxis right
ylim([0,100]);
ylabel("Duty cycle [\%]", args{:});
plot(time_1, out_1, 'LineWidth',1);
hold off;
xlim([0,100]);
set(gca,'FontSize',18);
xlabel("Time [s]", args{:});
legend({'y(t)','r(t)','out(t)'}, 'Location','north','Orientation', 'horizontal', args_1{:});
output_name = "figures/fig_a_1.pdf";
fig.PaperPositionMode = 'auto';
fig_pos = fig.PaperPosition;
fig.PaperSize = [fig_pos(3) fig_pos(4)];
print(fig,'-dpdf','-r300',char(output_name));

%% b) Non-distributed Control

data_0 = csvread("data/b_0.csv");
data_1 = csvread("data/b_1.csv");

time_0 = data_0(:,8) / 1000.0; 
lux_0  = data_0(:,1);
out_0  = data_0(:,2) * 100.0;
ref_0  = data_0(:,5);

time_1 = data_1(:,8) / 1000.0; 
lux_1  = data_1(:,1);
out_1  = data_1(:,2) * 100.0;
ref_1  = data_1(:,5);

e_0         = zeros(min(size(lux_0,1), size(lux_1,1)),1);
e_1         = zeros(min(size(lux_0,1), size(lux_1,1)),1);
nd_e_t      = zeros(min(size(lux_0,1), size(lux_1,1)),1);
c_err_0     = zeros(min(size(lux_0,1), size(lux_1,1)),1);
c_err_1     = zeros(min(size(lux_0,1), size(lux_1,1)),1);
nd_c_err_t  = zeros(min(size(lux_0,1), size(lux_1,1)),1);
c_var_0     = zeros(min(size(lux_0,1), size(lux_1,1)),1);
c_var_1     = zeros(min(size(lux_0,1), size(lux_1,1)),1);
nd_c_var_t  = zeros(min(size(lux_0,1), size(lux_1,1)),1);

sum = 0;
for j = 2:size(e_0,1)
    sum = sum + out_0(j-1)*(time_0(j) - time_0(j-1))/1000;
    e_0(j) = sum;
end;
sum = 0;
for j = 2:size(e_1,1)
    sum = sum + out_1(j-1)*(time_1(j) - time_0(j-1))/1000;
    e_1(j) = sum;
end;
nd_e_t = e_0 + e_1;

sum = 0;
for j = 1:size(c_err_0,1)
    sum = sum + max(ref_0(j) - lux_0(j), 0);
    c_err_0(j) = 1/j * sum;
end;
sum = 0;
for j = 1:size(c_err_1,1)
    sum = sum + max(ref_1(j) - lux_1(j), 0);
    c_err_1(j) = 1/j * sum;
end;
nd_c_err_t = c_err_0 + c_err_1;

sum = 0;
for j = 3:size(c_var_0,1)
    sum = sum + abs(lux_0(j) - 2*lux_0(j-1) + lux_0(j-2));
    c_var_0(j) = 1/j * sum / (0.100 * 0.100);
end;
sum = 0;
for j = 3:size(c_var_1,1)
    sum = sum + abs(lux_1(j) - 2*lux_1(j-1) + lux_1(j-2));
    c_var_1(j) = 1/j * sum / (0.100 * 0.100);
end;
nd_c_var_t = c_var_0 + c_var_1;

fig = figure('Name', "Non-Distributed Control Node 0");
hold on;
yyaxis left
ylim([0,100]);
ylabel("Illuminance [lx]", args{:});
plot(time_0, lux_0, 'LineWidth',1);
plot(time_0, ref_0, '-','Color', [0.9290 0.6940 0.1250], 'LineWidth',2);
yyaxis right
ylim([0,100]);
ylabel("Duty cycle [\%]", args{:});
plot(time_0, out_0, 'LineWidth',1);
hold off;
xlim([0,100]);
set(gca,'FontSize',18);
xlabel("Time [s]", args{:});
legend({'y(t)','r(t)','out(t)'}, 'Location','south','Orientation', 'horizontal', args_1{:});
output_name = "figures/fig_b_0.pdf";
fig.PaperPositionMode = 'auto';
fig_pos = fig.PaperPosition;
fig.PaperSize = [fig_pos(3) fig_pos(4)];
print(fig,'-dpdf','-r300',char(output_name));

fig = figure('Name', "Non-Distributed Control Node 1");
hold on;
yyaxis left
ylim([0,100]);
ylabel("Illuminance [lx]", args{:});
plot(time_1, lux_1, 'LineWidth',1);
plot(time_1, ref_1, '-','Color', [0.9290 0.6940 0.1250], 'LineWidth',2);
yyaxis right
ylim([0,100]);
ylabel("Duty cycle [\%]", args{:});
plot(time_1, out_1, 'LineWidth',1);
hold off;
xlim([0,100]);
set(gca,'FontSize',18);
xlabel("Time [s]", args{:});
legend({'y(t)','r(t)','out(t)'}, 'Location','north','Orientation', 'horizontal', args_1{:});
output_name = "figures/fig_b_1.pdf";
fig.PaperPositionMode = 'auto';
fig_pos = fig.PaperPosition;
fig.PaperSize = [fig_pos(3) fig_pos(4)];
print(fig,'-dpdf','-r300',char(output_name));

%% c) Metric comparison

args = {'interpreter','latex','FontSize',24};
args_1 = {'interpreter','latex','FontSize',22};

fig = figure('Name', "Energy comparison");
hold on;
plot(d_time_0, d_e_t, 'LineWidth',1);
plot(time_0, nd_e_t, 'LineWidth',1);
hold off;
ylabel('Energy [J]', args{:});
xlim([0,100]);
set(gca,'FontSize',22);
xlabel("Time [s]", args{:});
legend({'distributed','non-distributed'}, 'Location','northwest', args_1{:});
output_name = "figures/fig_c_0.pdf";
fig.PaperPositionMode = 'auto';
fig_pos = fig.PaperPosition;
fig.PaperSize = [fig_pos(3) fig_pos(4)];
print(fig,'-dpdf','-r300',char(output_name));

fig = figure('Name', "Comfort error comparison");
hold on;
plot(d_time_0, d_c_err_t, 'LineWidth',1);
plot(time_0, nd_c_err_t, 'LineWidth',1);
hold off;
ylim([0,5]);
ylabel('Comfort Error [lx]', args{:});
xlim([0,100]);
set(gca,'FontSize',22);
xlabel("Time [s]", args{:});
legend({'distributed','non-distributed'}, 'Location','northeast', args_1{:});
output_name = "figures/fig_c_1.pdf";
fig.PaperPositionMode = 'auto';
fig_pos = fig.PaperPosition;
fig.PaperSize = [fig_pos(3) fig_pos(4)];
print(fig,'-dpdf','-r300',char(output_name));

fig = figure('Name', "Comfort variance comparison");
hold on;
plot(d_time_0, d_c_var_t, 'LineWidth',1);
plot(time_0, nd_c_var_t, 'LineWidth',1);
hold off;
ylabel('Comfort Variance [lx/s$^2$]', args{:});
xlim([0,100]);
set(gca,'FontSize',22);
xlabel("Time [s]", args{:});
legend({'distributed','non-distributed'}, 'Location','northeast', args_1{:});
output_name = "figures/fig_c_2.pdf";
fig.PaperPositionMode = 'auto';
fig_pos = fig.PaperPosition;
fig.PaperSize = [fig_pos(3) fig_pos(4)];
print(fig,'-dpdf','-r300',char(output_name));