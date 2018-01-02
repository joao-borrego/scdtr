close all, clear all;

iterations = 50;
debug_output = false;

% The system
k11 = 0.64815; k12 = 0.24545;
k21 = 0.20696; k22 = 1.26932;
L1 = 100/3; o1 = 0.19328;
L2 = 200/3; o2 = 0.20200;

K = [k11, k12 ; k21 , k22];
L = [L1;L2]; o = [o1;o2];

% The cost function
c1 = 1.0; c2 = 1.0;
q1 = 0.0; q2 = 0.0;
c = [c1 c2]; Q = [q1 0; 0 q2];

% Preallocate arrays
d_0_1 = zeros(iterations, 2);
d_1_1 = zeros(iterations, 2);
d_2_1 = zeros(iterations, 2);
d_3_1 = zeros(iterations, 2);
d_4_1 = zeros(iterations, 2);
d_5_1 = zeros(iterations, 2);
d_0_2 = zeros(iterations, 2);
d_1_2 = zeros(iterations, 2);
d_2_2 = zeros(iterations, 2);
d_3_2 = zeros(iterations, 2);
d_4_2 = zeros(iterations, 2);
d_5_2 = zeros(iterations, 2);
d1_best = zeros(iterations, 2);
d2_best = zeros(iterations, 2);
z1 = zeros(iterations, 2);
z2 = zeros(iterations, 2);
d1_avg = zeros(iterations, 2);
d2_avg = zeros(iterations, 2);
y_1 = zeros(iterations, 2);
y_2 = zeros(iterations, 2);
min_best_1 = zeros(iterations,1);
min_best_2 = zeros(iterations,1);

% SOLVE WITH CONSENSUS
rho = 0.01;
%node 1 initialization
d1 = [0;0];
d1_av = [0;0];
d2_copy = [0;0];
y1 = [0;0];
k1 = [k11;k12]; 
%node 2 initialization
d2 = [0;0];
d2_av = [0;0];
d1_copy = [0;0];
y2 = [0;0];
k2 = [k21;k22]; 

% Main loop
for i=1:iterations
   % node 1
   d11_best = -1;
   d12_best = -1;
   min_best_1(i) = 100000; %big number
   sol_unconstrained = 1;
   sol_boundary_linear = 1;
   sol_boundary_0 = 1;
   sol_boundary_100 = 1;
   sol_linear_0 = 1;
   sol_linear_100 = 1;
   z11 = -c1 - y1(1) + rho*d1_av(1);
   z12 = -y1(2) + rho*d1_av(2);
   u1 = o1-L1;
   u2 = 0;
   u3 = 100;
   p11 = 1/(rho+q1);
   p12 = 1/rho;
   n = k11*k11*p11 + k12*k12*p12;
   w1 = -k11*p11*z11-k12*z12*p12;
   w2 = -z11*p11;
   w3 = z11*p11;

   z1(i, :) = [z11; z12];
   
   %compute unconstrained minimum
   d11u = p11*z11;
   d12u = p12*z12;

   d_0_1(i, :) = [d11u; d12u];

   %check feasibility of unconstrained minimum using local constraints
   if (d11u < 0), sol_unconstrained = 0; end
   if (d11u > 100), sol_unconstrained = 0; end
   if (k11*d11u + k12*d12u < L1-o1), sol_unconstrained = 0; end
   
   % compute function value and if best store new optimum
   if sol_unconstrained 
        min_unconstrained = 0.5*q1*d11u^2 + c1*d11u + y1(1)*(d11u-d1_av(1)) + ...
           y1(2)*(d12u-d1_av(2)) + rho/2*(d11u-d1_av(1))^2 + rho/2*(d12u-d1_av(2))^2;
       if min_unconstrained < min_best_1(i)
           d11_best = d11u;
           d12_best = d12u;
           min_best_1(i) = min_unconstrained;
       end
   end
   %compute minimum constrained to linear boundary   
   d11bl = p11*z11+p11*k11/n*(w1-u1);
   d12bl = p12*z12+p12*k12/n*(w1-u1);

   d_1_1(i, :) = [d11bl; d12bl];

   %check feasibility of minimum constrained to linear boundary
   if (d11bl < 0), sol_boundary_linear = 0; end
   if (d11bl > 100), sol_boundary_linear = 0; end
   % compute function value and if best store new optimum
   if sol_boundary_linear 
        min_boundary_linear = 0.5*q1*d11bl^2 + c1*d11bl + y1(1)*(d11bl-d1_av(1)) + ...
           y1(2)*(d12bl-d1_av(2)) + rho/2*(d11bl-d1_av(1))^2 + rho/2*(d12bl-d1_av(2))^2;
       if min_boundary_linear < min_best_1(i)
           d11_best = d11bl;
           d12_best = d12bl;
           min_best_1(i) = min_boundary_linear;
       end
   end
   %compute minimum constrained to 0 boundary
   d11b0 = 0;
   d12b0 = p12*z12;

   d_2_1(i, :) = [d11b0; d12b0];

   %check feasibility of minimum constrained to 0 boundary
   if (d11b0 > 100), sol_boundary_0 = 0; end
   if (k11*d11b0 + k12*d12b0 < L1-o1), sol_boundary_0 = 0; end
   % compute function value and if best store new optimum
   if sol_boundary_0 
        min_boundary_0 = 0.5*q1*d11b0^2 + c1*d11b0 + y1(1)*(d11b0-d1_av(1)) + ...
           y1(2)*(d12b0-d1_av(2)) + rho/2*(d11b0-d1_av(1))^2 + rho/2*(d12b0-d1_av(2))^2;
       if min_boundary_0 < min_best_1(i)
           d11_best = d11b0;
           d12_best = d12b0;
           min_best_1(i) = min_boundary_0;
       end
   end
   %compute minimum constrained to 100 boundary
   d11b100 = 100;
   d12b100 = p12*z12;

   d_3_1(i, :) = [d11b100; d12b100];

   %check feasibility of minimum constrained to 100 boundary
   if (d11b0 < 0), sol_boundary_100 = 0; end
   if (k11*d11b100 + k12*d12b100 < L1-o1), sol_boundary_100 = 0; end
   % compute function value and if best store new optimum
   if sol_boundary_100 
        min_boundary_100 = 0.5*q1*d11b100^2 + c1*d11b100 + y1(1)*(d11b100-d1_av(1)) + ...
           y1(2)*(d12b100-d1_av(2)) + rho/2*(d11b100-d1_av(1))^2 + rho/2*(d12b100-d1_av(2))^2;
       if min_boundary_100 < min_best_1(i)
           d11_best = d11b100;
           d12_best = d12b100;
           min_best_1(i) = min_boundary_100;
       end
   end
   % compute minimum constrained to linear and zero boundary
   common = (rho+q1)/((rho+q1)*n-k11*k11);
   det1 = common;
   det2 = -k11*common;
   det3 = det2;
   det4 = n*(rho+q1)*common;
   x1 = det1*w1 + det2*w2;
   x2 = det3*w1 + det4*w2;
   v1 = det1*u1 + det2*u2; %u2 = 0 so this can be simplified
   v2 = det3*u1 + det4*u2; %u2 = 0 so this can be simplified
   d11l0 = p11*z11+p11*k11*(x1-v1)+p11*(x2-v2);
   d12l0 = p12*z12+p12*k12*(x1-v1);

   d_4_1(i, :) = [d11l0; d12l0];

   %check feasibility
   if (d11l0 > 100), sol_linear_0 = 0; end
   % compute function value and if best store new optimum
   if sol_linear_0 
        min_linear_0 = 0.5*q1*d11l0^2 + c1*d11l0 + y1(1)*(d11l0-d1_av(1)) + ...
           y1(2)*(d12l0-d1_av(2)) + rho/2*(d11l0-d1_av(1))^2 + rho/2*(d12l0-d1_av(2))^2;
       if min_linear_0 < min_best_1(i)
           d11_best = d11l0;
           d12_best = d12l0;
           min_best_1(i) = min_linear_0;
       end
   end
   % compute minimum constrained to linear and 100 boundary
   common = (rho+q1)/((rho+q1)*n-k11*k11);
   det1 = common;
   det2 = k11*common;
   det3 = det2;
   det4 = n*(rho+q1)*common;
   x1 = det1*w1 + det2*w3;
   x2 = det3*w1 + det4*w3;
   v1 = det1*u1 + det2*u3; 
   v2 = det3*u1 + det4*u3; 
   d11l100 = p11*z11+p11*k11*(x1-v1)-p11*(x2-v2);
   d12l100 = p12*z12+p12*k12*(x1-v1);

   d_5_1(i, :) = [d11l100; d12l100];

   %check feasibility
   if (d11l100 < 0), sol_linear_100 = 0; end
   % compute function value and if best store new optimum
   if sol_linear_100 
        min_linear_100 = 0.5*q1*d11l100^2 + c1*d11l100 + y1(1)*(d11l100-d1_av(1)) + ...
           y1(2)*(d12l100-d1_av(2)) + rho/2*(d11l100-d1_av(1))^2 + rho/2*(d12l100-d1_av(2))^2;
       if min_linear_100 < min_best_1(i)
           d11_best = d11u;
           d12_best = d12u;
           min_best_1(i) = min_linear100;
       end
   end
   %store data and save for next cycle
   best_d11(i) = d11_best;
   best_d12(i) = d12_best;
   d1 = [d11_best;d12_best];
   
   d1_best(i, :) = d1;

   % node 2 

   d21_best = -1;
   d22_best = -1;
   min_best_2(i) = 100000; %big number
   sol_unconstrained = 1;
   sol_boundary_linear = 1;
   sol_boundary_0 = 1;
   sol_boundary_100 = 1;
   sol_linear_0 = 1;
   sol_linear_100 = 1;
   z22 = -c2 - y2(2) + rho*d2_av(2);
   z21 = -y2(1) + rho*d2_av(1);
   u1 = o2-L2;
   u2 = 0;
   u3 = 100;
   p22 = 1/(rho+q2);
   p21 = 1/rho;
   n = k22*k22*p22 + k21*k21*p21;
   w1 = -k22*p22*z22-k21*z21*p21;
   w2 = -z22*p22;
   w3 = z22*p22;

   z2(i, :) = [z21; z22];
   
   %compute unconstrained minimum
   d21u = p21*z21;
   d22u = p22*z22;

   d_0_2(i, :) = [d21u; d22u];

   %check feasibility of unconstrained minimum using local constraints
   if (d22u < 0), sol_unconstrained = 0; end
   if (d22u > 100), sol_unconstrained = 0; end
   if (k21*d21u + k22*d22u < L2-o2), sol_unconstrained = 0; end

   % compute function value and if best store new optimum
   if sol_unconstrained 
        min_unconstrained = 0.5*q2*d22u^2 + c2*d22u + y2(1)*(d21u-d2_av(1)) + ...
           y2(2)*(d22u-d2_av(2)) + rho/2*(d21u-d2_av(1))^2 + rho/2*(d22u-d2_av(2))^2;
       if min_unconstrained < min_best_2(i)
           d21_best = d21u;
           d22_best = d22u;
           min_best_2(i) = min_unconstrained;
       end
   end
   %compute minimum constrained to linear boundary   
   d21bl = p21*z21+p21*k21/n*(w1-u1);
   d22bl = p22*z22+p22*k22/n*(w1-u1);

   d_1_2(i, :) = [d21bl; d22bl];

   %check feasibility of minimum constrained to linear boundary
   if (d22bl < 0), sol_boundary_linear = 0; end
   if (d22bl > 100), sol_boundary_linear = 0; end
   % compute function value and if best store new optimum
   if sol_boundary_linear 
        min_boundary_linear = 0.5*q2*d22bl^2 + c2*d22bl + y2(1)*(d21bl-d2_av(1)) + ...
           y2(2)*(d22bl-d2_av(2)) + rho/2*(d21bl-d2_av(1))^2 + rho/2*(d22bl-d2_av(2))^2;
       if min_boundary_linear < min_best_2(i)
           d21_best = d21bl;
           d22_best = d22bl;
           min_best_2(i) = min_boundary_linear;
       end
   end
   %compute minimum constrained to 0 boundary
   d22b0 = 0;
   d21b0 = p21*z21;
   
   d_2_2(i, :) = [d21b0; d22b0];

   %check feasibility of minimum constrained to 0 boundary
   if (k21*d21b0 + k22*d22b0 < L2-o2), sol_boundary_0 = 0; end
   if (d22b0 > 100), sol_boundary_0 = 0; end
   % compute function value and if best store new optimum
   if sol_boundary_0 
        min_boundary_0 = 0.5*q2*d22b0^2 + c2*d22b0 + y2(1)*(d21b0-d2_av(1)) + ...
           y2(2)*(d22b0-d2_av(2)) + rho/2*(d21b0-d2_av(1))^2 + rho/2*(d22b0-d2_av(2))^2;
       if min_boundary_0 < min_best_2(i)
           d21_best = d21b0;
           d22_best = d22b0;
           min_best_2(i) = min_boundary_0;
       end
   end
   %compute minimum constrained to 100 boundary
   d22b100 = 100;
   d21b100 = p21*z21;

   d_3_2(i, :) = [d21b100; d22b100];

   %check feasibility of minimum constrained to 100 boundary
   if (k21*d21b100 + k22*d22b100 < L2-o2), sol_boundary_100 = 0; end
   if (d22b100 < 0), sol_boundary_100 = 0; end
   % compute function value and if best store new optimum
   if sol_boundary_100 
        min_boundary_100 = 0.5*q2*d22b100^2 + c2*d22b100 + y2(1)*(d21b100-d2_av(1)) + ...
           y2(2)*(d22b100-d2_av(2)) + rho/2*(d21b100-d2_av(1))^2 + rho/2*(d22b100-d2_av(2))^2;
       if min_boundary_100 < min_best_2(i)
           d21_best = d21b100;
           d22_best = d22b100;
           min_best_2(i) = min_boundary_100;
       end
   end
   % compute minimum constrained to linear and zero boundary
   common = (rho+q2)/((rho+q2)*n-k22*k22);
   det1 = common;
   det2 = -k22*common;
   det3 = det2;
   det4 = n*(rho+q2)*common;
   x1 = det1*w1 + det2*w2;
   x2 = det3*w1 + det4*w2;
   v1 = det1*u1 + det2*u2; %u2 = 0 so this can be simplified
   v2 = det3*u1 + det4*u2; %u2 = 0 so this can be simplified
   d22l0 = p22*z22+p22*k22*(x1-v1)+p22*(x2-v2);
   d21l0 = p21*z21+p21*k21*(x1-v1);
   
   d_4_2(i, :) = [d21l0; d22l0];

   %check feasibility
   if(d22l0 > 100), sol_linear_0 = 0; end
   % compute function value and if best store new optimum
   if sol_linear_0 
        min_linear_0 = 0.5*q2*d22l0^2 + c2*d22l0 + y2(1)*(d21l0-d2_av(1)) + ...
           y2(2)*(d22l0-d2_av(2)) + rho/2*(d21l0-d2_av(1))^2 + rho/2*(d22l0-d2_av(2))^2;
       if min_linear_0 < min_best_2(i)
           d21_best = d21l0;
           d22_best = d22l0;
           min_best_2(i) = min_linear_0;
       end
   end
   % compute minimum constrained to linear and 100 boundary
   common = (rho+q2)/((rho+q2)*n-k22*k22);
   det1 = common;
   det2 = k22*common;
   det3 = det2;
   det4 = n*(rho+q2)*common;
   x1 = det1*w1 + det2*w3;
   x2 = det3*w1 + det4*w3;
   v1 = det1*u1 + det2*u3; 
   v2 = det3*u1 + det4*u3; 
   d22l100 = p22*z22+p22*k22*(x1-v1)-p22*(x2-v2);
   d21l100 = p21*z21+p21*k21*(x1-v1);
   
   d_5_2(i, :) = [d21l100; d22l100];

   %check feasibility
   if (d22l100 < 0), sol_linear_100 = 0; end
   %now must choose the minimum among the feasible solutions
   % compute function value and if best store new optimum
   if sol_linear_100 
        min_linear_100 = 0.5*q2*d22l100^2 + c2*d22l100 + y2(1)*(d21l100-d2_av(1)) + ...
           y2(2)*(d22l100-d2_av(2)) + rho/2*(d21l100-d2_av(1))^2 + rho/2*(d22l100-d2_av(2))^2;
       if min_linear_100 < min_best_2(i)
           d21_best = d21u;
           d22_best = d22u;
           min_best_2(i) = min_linear100;
       end
   end
   %store data and save for next cycle
   best_d21(i) = d21_best;
   best_d22(i) = d22_best;
   d2 = [d21_best;d22_best];

   d2_best(i, :) = d2;

   % Update

   % Communication
   % send node 1 solution to neighboors
   d1_copy = d1;
   % send solution to neighbors
   d2_copy = d2;
  
   %compute average with available knowledge
   d1_av = (d1+d2_copy)/2;
   %update local lagrangian
   y1 = y1 + rho*(d1-d1_av);
   % Compute average with available data
   d2_av = (d1_copy+d2)/2;
   % Update local lagrangian
   y2 = y2 + rho*(d2-d2_av);
   
   y_1(i, :) = y1;
   y_2(i, :) = y2;
   d1_avg(i, :) = d1_av;
   d2_avg(i, :) = d2_av;

end


%% Debug output

if debug_output
    folder = "data/";
    csvwrite(folder + "d_0_1.csv", d_0_1);
    csvwrite(folder + "d_1_1.csv", d_1_1);
    csvwrite(folder + "d_2_1.csv", d_2_1);
    csvwrite(folder + "d_3_1.csv", d_3_1);
    csvwrite(folder + "d_4_1.csv", d_4_1);
    csvwrite(folder + "d_5_1.csv", d_5_1);
    csvwrite(folder + "d_0_2.csv", d_0_2);
    csvwrite(folder + "d_1_2.csv", d_1_2);
    csvwrite(folder + "d_2_2.csv", d_2_2);
    csvwrite(folder + "d_3_2.csv", d_3_2);
    csvwrite(folder + "d_4_2.csv", d_4_2);
    csvwrite(folder + "d_5_2.csv", d_5_2);
    csvwrite(folder + "d1_best.csv", d1_best);
    csvwrite(folder + "d2_best.csv", d2_best);
    csvwrite(folder + "z1.csv", z1);
    csvwrite(folder + "z2.csv", z2);
    csvwrite(folder + "d1_avg.csv", d1_avg);
    csvwrite(folder + "d2_avg.csv", d2_avg);
    csvwrite(folder + "y_1.csv", y_1);
    csvwrite(folder + "y_2.csv", y_2);
    csvwrite(folder + "min_best_1.csv", min_best_1);
    csvwrite(folder + "min_best_2.csv", min_best_2);
end

%% Plot figures

% Configs
set(0,'DefaultTextFontname', 'CMU Serif');
set(0,'DefaultAxesFontName', 'CMU Serif');
args = {'interpreter','latex','FontSize',24};

%% d1 and d2 solutions over time

files = ["fig_d_1_best","fig_d_2_best"];
names = ["d_1_best","d_2_best"];

time = 1:iterations;
for i = 1:2
    fig = figure('Name',names(i));
    if (i == 1)
        plot(time, d1_best);
    else
        plot(time,d2_best);
    end
    set(gca,'FontSize',22);
    xlabel("Iterations", args{:});
    ylabel("Duty Cycle [\%]", args{:});
    axis([0,50,0,100]);
    legend({'$d_{1}$','$d_{2}$'}, 'Location','southeast','Orientation','horizontal',args{:});
    output_name = sprintf("figures/%s.pdf", files(i));
    fig.PaperPositionMode = 'auto';
    fig_pos = fig.PaperPosition;
    fig.PaperSize = [fig_pos(3) fig_pos(4)];
    print(fig,'-dpdf','-r300',char(output_name));
end

%% Cost function

time = 1:iterations;
fig = figure('Name','Cost function');
hold on;
plot(time,min_best_1);
plot(time,min_best_2);
hold off;
set(gca,'FontSize',22);
xlabel("Iterations", args{:});
ylabel("Cost", args{:});
axis([0,50,0,70]);
legend({'cost$_{1}$','cost$_{2}$'}, 'Location','southeast','Orientation','horizontal',args{:});
output_name = "figures/fig_min_cost.pdf";
fig.PaperPositionMode = 'auto';
fig_pos = fig.PaperPosition;
fig.PaperSize = [fig_pos(3) fig_pos(4)];
print(fig,'-dpdf','-r300',char(output_name));

%% Scatter plot of solutions d_2(d_1)

time = 1:100;
fig = figure('Name','Solution evolution');
hold on;

% Cost function contour lines
[x,y] = meshgrid(time,time);
z = c1*x+c2*y+q1*x.^2+q2*y.^2;
contour(x,y,z,10);
% Constraint lines
ct1_1 = (L1-o1)/k12-(k11/k12)*time;
ct1_2 = (L2-o2)/k22-(k21/k22)*time;
plot(time,ct1_1, time,ct1_2, 'LineWidth',2);
% Solutions over time - d1_avg = d2_avg
plot(d1_avg(:,1),d2_avg(:,2),'-','LineWidth',2);
%plot(d1_avg(:,1),d2_avg(:,2),'bx');
% Optimum solution
plot(d1_avg(end,1),d2_avg(end,2),'k*','MarkerSize',10,'LineWidth',1.3)
optimum_str = sprintf("(%.2f, %.2f)", d1_avg(end,1), d2_avg(end,2));
args = {'HorizontalAlignment','left','interpreter','latex','FontSize',24};
text(d1_avg(end,1)+4,d2_avg(end,2)+4,char(optimum_str), args{:});
% Labels
set(gca,'FontSize',22);
xlabel("$d_1$ [\%]", args{:});
ylabel("$d_2$ [\%]", args{:});
axis([0,100,0,100]);
hold off;
% Save
output_name = "figures/fig_scatter.pdf";
fig.PaperPositionMode = 'auto';
fig_pos = fig.PaperPosition;
fig.PaperSize = [fig_pos(3) fig_pos(4)];
print(fig,'-dpdf','-r300',char(output_name));