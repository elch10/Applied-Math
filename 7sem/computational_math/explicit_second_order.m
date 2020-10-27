%a = 0;
%b = 1;
T = 4;
format shortG;

for n = [100]
    n
    xs = linspace(0, 1, n+1);
    prev_t = 0;
    tau = 1 / (4 * n .^ 2);
    U_prev = U_0(xs).';
    
    res = U_prev;
    real = U_prev.';
    for t = tau:tau:T-tau
        U_prev = explicit_algo(xs, t, tau, @U_a, @U_b, U_prev, @F);
        res = cat(2, res, U_prev);
        real = cat(1, real, U(xs, t));
    end
    figure;
    contour(abs(res.' - real), 'ShowText', 'on');
    max(max(abs(res.' - real)))
end

function res = U(x, t)
    res = exp(x) .* sin(x .* t);
end

function res_a = U_a(t)
    res_a = U(0, t);
end

function res_b = U_b(t)
    res_b = U(1, t);
end

function res_0 = U_0(x)
    res_0 = U(x, 0);
end

function res_f = F(x, t)
    res_f = exp(x) .* (cos(x .* t) .* (x - 2 * t) - sin(x .* t) .* (1 - t .^ 2));
end


function next_time = explicit_algo(xs, next_t, tau, U_a, U_b, U_prev, F)
    h = xs(2) - xs(1);
    x_len = numel(xs) - 2;
    
    next_time = zeros(numel(xs), 1);
    next_time(1) = U_a(next_t);
    next_time(numel(xs)) = U_b(next_t);
    
    for i=2:x_len+1
        diff = (U_prev(i+1) - 2 * U_prev(i) + U_prev(i-1)) / (h .^ 2);
        next_time(i) = tau * diff + tau * F(xs(i), next_t - tau) + U_prev(i);
    end
end