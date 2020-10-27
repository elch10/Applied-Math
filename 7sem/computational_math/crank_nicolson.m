%a = 0;
%b = 1;
T = 4;
delta = 0.5;
format shortG;

for n = [100]
    n
    xs = linspace(0, 1, n+1);
    prev_t = 0;
    tau = 1 / n;
    U_prev = U_0(xs).';
    
    res = U_prev;
    real = U_prev.';
    for t = tau:tau:T-tau
        U_prev = crank_nicolson_algo(xs, t, tau, delta, @U_a, @U_b, U_prev, @F);
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

%function next_time = explicit_method(xs, next_t, tau, U_a, U_b, U_prev, F)
%end

function next_time = crank_nicolson_algo(xs, next_t, tau, delta, U_a, U_b, U_prev, F)
    h = xs(2) - xs(1);
    x_len = numel(xs) - 2;

    A_l = repelem(-delta*tau, x_len-1);
    A_l = A_l(:);
    B_l = repelem(h*h + 2*tau*delta, x_len);
    B_l = B_l(:);
    C_l = repelem(-delta*tau, x_len-1);
    C_l = C_l(:);

    IN = cat(2, 1:x_len, 1:x_len-1, 2:x_len);
    JN = cat(2, 1:x_len, 2:x_len, 1:x_len-1);
    VN = cat(2, repelem(h*h - 2*tau*(1-delta), x_len), repelem((1-delta)*tau, 2*(x_len-1)));
    C_r = sparse(IN, JN, VN);

    right_side = C_r * U_prev(2:x_len+1) + tau * h.^2 * F(xs(2:x_len+1), next_t-tau/2).';

    right_side(1) = right_side(1) + (1-delta)*tau*U_prev(1) + delta*tau*U_a(next_t);
    mm = numel(right_side);
    right_side(mm) = right_side(mm) + (1-delta)*tau*U_prev(x_len+2) + delta*tau*U_b(next_t);

    nxt = sweep(A_l, B_l, C_l, right_side);

    next_time = cat(1, U_a(next_t), nxt, U_b(next_t));
end
