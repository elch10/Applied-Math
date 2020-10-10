%a = 2;
%b = 4;
T = 5;

function res = U(x, t)
    res = x - 2 .* t .* x + 5 .* sin(t);
end

function res_a = U_a(t)
    a = 2;
    res_a = U(a, t);
end

function res_b = U_b(t)
    b = 4;
    res_b = U(b, t);
end

function res_0 = U_0(x)
    res_0 = U(x, 0);
end

function res_f = F(x, t)
    res_f = exp(-x) .* t + x - t;
end

function next_time = crank_nicolson(xs, next_t, tau, delta, U_a, U_b, U_prev, F)
    h = xs(2) - xs(1);
    x_len = numel(xs) - 2;

    C_l = diag(repelem(h*h + 2*tau*delta, x_len));
    C_l = C_l + diag(repelem(-delta*tau, x_len-1), 1);
    C_l = C_l + diag(repelem(-delta*tau, x_len-1), -1);

    C_r = diag(repelem(h*h - 2*tau*(1-delta), x_len));
    C_r = C_r + diag(repelem((1-delta)*tau, x_len-1), 1);
    C_r = C_r + diag(repelem((1-delta)*tau, x_len-1), -1);

    right_side = C_r * U_prev(2:x_len+1) + F(xs(2:x_len+1), next_t-tau/2).' ...
        + cat(2, U_a(next_t), zeros(1, x_len-2), U_b(next_t)).';
    nxt = linsolve(C_l, right_side);
    next_time = cat(1, U_a(next_t), nxt, U_b(next_t));
end

xs = linspace(2, 4, 101);
prev_t = 0;
tau = 0.02;
delta = 1;
U_prev = U_0(xs).';

res = U_prev;
real = U_prev.';
for t = tau:tau:T-tau
    U_prev = crank_nicolson(xs, t, tau, delta, @U_a, @U_b, U_prev, @F);
    res = cat(2, res, U_prev);
    real = cat(1, real, U(xs, t));
end

disp('Результат:')
format shortG
%contour(res, 'ShowText', 'on')
contour(abs(res.' - real), 'ShowText', 'on')

