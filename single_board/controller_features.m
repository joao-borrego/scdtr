%% Configs

set(0,'DefaultTextFontname', 'CMU Serif')
set(0,'DefaultAxesFontName', 'CMU Serif')
args = {'interpreter','latex','FontSize',26};

%% h) Feedforward comparison

files = ["fig_h_0",
         "fig_h_0_5",
         "fig_h_1"];
names = ["h.1) FF K_F = 0",
         "h.2) FF K_F = 0.5",
         "h.3) FF K_F = 1"];

for i = 1:3
    data = csvread(strcat("data/",files(i),".csv"));
    time = (data(:,5) / 1000.0) - 4.5;
    ref  = data(:,1);
    lx   = data(:,3);
    fig = figure('Name', names(i));
    hold on;
    plot(time, ref, time, lx, 'LineWidth',1.3);
    hold off;
    set(gca,'FontSize',24);
    xlabel("Time [s]", args{:});
    axis([0, 1, 20, 80]);
    ylabel("Illuminance [lx]", args{:});
    legend({'ref(t)','y(t)'}, 'Location','southeast',args{:});
    output_name = sprintf("figures/%s.pdf",files(i));
    fig.PaperPositionMode = 'auto';
    fig_pos = fig.PaperPosition;
    fig.PaperSize = [fig_pos(3) fig_pos(4)];
    print(fig,'-dpdf','-r300',char(output_name));
end

%% i) FF + Delayed PI comparison

files = ["fig_i_1",
         "fig_i_2",
         "fig_i_3"];
names = ["i.1) Delay = 1 Sample, FF K_F = 0.5",
         "i.2) Delay = 2 Samples, FF K_F = 0.5",
         "i.3) Delay = 3 Samples, FF K_F = 0.5"];

for i = 1:3
    data = csvread(strcat("data/",files(i),".csv"));
    time = (data(:,5) / 1000.0) - 4.5;
    ref  = data(:,1);
    lx   = data(:,3);
    fig = figure('Name', names(i));
    hold on;
    plot(time, ref, time, lx, 'LineWidth',1.3);
    hold off;
    set(gca,'FontSize',24);
    xlabel("Time [s]", args{:});
    axis([0, 1, 20, 80]);
    ylabel("Illuminance [lx]", args{:});
    legend({'ref(t)','y(t)'}, 'Location','southeast',args{:});
    output_name = sprintf("figures/%s.pdf",files(i));
    fig.PaperPositionMode = 'auto';
    fig_pos = fig.PaperPosition;
    fig.PaperSize = [fig_pos(3) fig_pos(4)];
    print(fig,'-dpdf','-r300',char(output_name));
end

%% j) Anti-windup comparison

files = ["fig_j_1",
         "fig_j_2",
         "fig_j_3"];
names = ["j.1) Anti-windup off",
         "j.2) Simple Anti-windup",
         "j.3) Feedback Anti-windup"];

for i = 1:3
    data = csvread(strcat("data/",files(i),".csv"));
    time = (data(:,5) / 1000.0) - 4.0;
    ref  = data(:,1);
    lx   = data(:,3);
    fig = figure('Name', names(i));
    hold on;
    plot(time, ref, time, lx, 'LineWidth',1.3);
    hold off;
    set(gca,'FontSize',24);
    xlabel("Time [s]", args{:});
    ylabel("Illuminance [lx]", args{:});
    ylim([20, 110])
    legend({'ref(t)','y(t)'}, 'Location','northeast',args{:});
    output_name = sprintf("figures/%s.pdf",files(i));
    fig.PaperPositionMode = 'auto';
    fig_pos = fig.PaperPosition;
    fig.PaperSize = [fig_pos(3) fig_pos(4)];
    print(fig,'-dpdf','-r300',char(output_name));
end

%% k) Error deadzone comparison

files = ["fig_k_1",
         "fig_k_2",
         "fig_k_3"];
names = ["k.1) Error deadzone off",
         "k.2) Error deadzone [-0.5,0.5]",
         "k.3) Error deadzone [-1.6,1.6]"];

for i = 1:3
    data = csvread(strcat("data/",files(i),".csv"));
    time = (data(:,5) / 1000.0) - 4.0;
    ref  = data(:,1);
    out   = data(:,2) * 100.0;
    fig = figure('Name', names(i));
    hold on;
    yyaxis left
    ylim([25, 55])
    ylabel("Illuminance [lx]", args{:});
    plot(time, ref, 'LineWidth',1.3);
    yyaxis right
    ylim([40, 85])
    ylabel("Duty cycle [\%]", args{:});
    plot(time, out, 'LineWidth',1.3);
    hold off;
    set(gca,'FontSize',24);
    xlabel("Time [s]", args{:});
    ylim([30, 100])
    legend({'ref(t)','out(t)'}, 'Location','southeast',args{:});
    output_name = sprintf("figures/%s.pdf",files(i));
    fig.PaperPositionMode = 'auto';
    fig_pos = fig.PaperPosition;
    fig.PaperSize = [fig_pos(3) fig_pos(4)];
    print(fig,'-dpdf','-r300',char(output_name));
end
