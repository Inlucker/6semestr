close all
clear
clc
X=[-13.40 -12.63 -13.65 -14.23 -13.39 -12.36 -13.52 -13.44 -13.87 -11.82 -12.01 -11.40 -13.02 -12.61 -13.06 -13.75 -13.55 -14.01 -11.75 -12.95 -12.59 -13.60 -12.76 -11.05 -13.15 -13.61 -11.73 -13.00 -12.66 -12.67 -12.60 -12.47 -13.52 -12.61 -11.93 -13.11 -13.22 -11.87 -13.44 -12.70 -11.78 -12.30 -12.89 -13.29 -12.48 -10.44 -12.55 -12.64 -12.03 -14.60 -14.56 -13.30 -11.32 -12.24 -11.17 -12.50 -13.25 -12.55 -12.85 -12.67 -12.41 -12.58 -12.10 -13.54 -12.69 -12.87 -12.71 -12.77 -13.30 -12.74 -12.73 -12.64 -12.18 -11.20 -12.40 -13.78 -13.71 -10.74 -11.89 -13.20 -11.31 -14.26 -10.38 -12.88 -11.39 -11.35 -12.55 -12.84 -10.25 -12.40 -14.01 -11.47 -13.14 -12.69 -11.92 -12.86 -13.06 -12.57 -13.63 -12.34 -12.84 -14.03 -13.34 -11.64 -13.58 -10.44 -11.37 -11.01 -13.80 -13.27 -12.32 -10.69 -12.92 -13.29 -12.58 -13.98 -11.46 -11.82 -12.33 -11.47];
mu = get_mu(X);
Ssqr = get_Ssqr(X);
gamma = input("Input gamma: ");
dov_interval_mu = get_dov_interval_mu(X, 0.9);
mu_lower = dov_interval_mu(1);
mu_upper = dov_interval_mu(2);
dov_interval_sigma = get_dov_interval_sigma(X, 0.9);
sigma_lower = dov_interval_sigma(1);
sigma_upper = dov_interval_sigma(2);
fprintf("mu =  %.3f\n", mu);
fprintf("Ssqr = %.3f\n", Ssqr);
fprintf('mu_lower = %.3f\n', mu_lower);
fprintf('mu_upper = %.3f\n', mu_upper);
fprintf('sigma_lower = %.3f\n', sigma_lower);
fprintf('sigma_upper = %.3f\n', sigma_upper);
mu_lower_y = [];
mu_upper_y = [];
mu_y = [];
n_x = [];
muN_y = [];
n = size(X, 2);
for i=1:n
    tmp = get_dov_interval_mu_N(X, gamma, i);
    mu_lower_y = [mu_lower_y tmp(1)];
    mu_upper_y = [mu_upper_y tmp(2)];
    Xn = X(1:i);
    mu_y = [mu_y get_mu(Xn)];
    muN_y = [muN_y mu];
    n_x = [n_x i];
end
figure('Position', [180 200 560 420]);
hold on;
plot(n_x, mu_lower_y);
plot(n_x, mu_upper_y);
plot(n_x, mu_y);
plot(n_x, muN_y);
grid;
hold off;
sigma_lower_y = [];
sigma_upper_y = [];
sigma_y = [];
n_x = [];
sigmaN_y = [];
for i=1:n
    tmp = get_dov_interval_sigma_N(X, gamma, i);
    sigma_lower_y = [sigma_lower_y tmp(1)];
    sigma_upper_y = [sigma_upper_y tmp(2)];
    Xn = X(1:i);
    sigma_y = [sigma_y get_Ssqr(Xn)];
    sigmaN_y = [sigmaN_y Ssqr];
    n_x = [n_x i];
end
figure('Position', [780 200 560 420]);
hold on;
plot(n_x, sigma_lower_y);
plot(n_x, sigma_upper_y);
plot(n_x, sigma_y);
plot(n_x, sigmaN_y);
grid;
hold off;


function mu = get_mu(X)
    mu = sum(X) / size(X, 2);
end

function Ssqr = get_Ssqr(X)
    n = size(X, 2);
    mu = get_mu(X);
    Ssqr = 1/(n-1)*sum(power(X-mu, 2));
end

function dov_interval_mu = get_dov_interval_mu(X, gamma)
    mu = get_mu(X);
    Ssqr = get_Ssqr(X);
    n = size(X, 2);
    alpha = 1-(1-gamma)/2;
    mu_lower = mu - (sqrt(Ssqr)*tinv(alpha, n-1)/sqrt(n));
    mu_upper = mu + (sqrt(Ssqr)*tinv(alpha, n-1)/sqrt(n));
    dov_interval_mu = [mu_lower mu_upper];
end

function dov_interval_sigma = get_dov_interval_sigma(X, gamma)
    Ssqr = get_Ssqr(X);
    n = size(X, 2);
    alpha2 = (1-gamma)/2;
    alpha1 = (1+gamma)/2;
    sigma_lower = (n-1)*Ssqr/chi2inv(alpha1, n-1);
    sigma_upper = (n-1)*Ssqr/chi2inv(alpha2, n-1);
    dov_interval_sigma = [sigma_lower sigma_upper];
end

function dov_interval_mu = get_dov_interval_mu_N(X, gamma, N)
    Xn = X(1:N);
    dov_interval_mu = get_dov_interval_mu(Xn, gamma);
end

function dov_interval_sigma = get_dov_interval_sigma_N(X, gamma, N)
    Xn = X(1:N);
    dov_interval_sigma = get_dov_interval_sigma(Xn, gamma);
end