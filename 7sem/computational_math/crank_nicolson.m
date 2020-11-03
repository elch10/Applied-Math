%a = 0;
%b = 1;
T = 1;
delta = 0.5;
format shortG;
max_errors_implicit = [];
taus = logspace(-4, -1, 4);

for tau=taus
    tau
    h = sqrt(2.1 * tau);
    xs = 0:h:1;
    prev_t = 0;
    U_prev = U_0(xs).';

    res = U_prev;
    real = U_prev.';
    for t = tau:tau:T-tau
        U_prev = crank_nicolson_algo(xs, t, tau, delta, U_prev, @F);
        res = cat(2, res, U_prev);
        real = cat(1, real, U(xs, t));
    end
%     figure;
%     contour(abs(res.' - real), 'ShowText', 'on');
    max_errors_implicit = [max_errors_implicit max(max(abs(res.' - real)))];
end

figure;
loglog(taus, max_errors_implicit, '-o', taus, 2.1 * taus, '-o');
grid on;
legend('CN', 'h^2');
xlabel('tau');
ylabel('max error');

% figure
% loglog(taus, max_errors_explicit, '-o', taus, max_errors_implicit, '-o', ...
% taus, max_errors_cn, '-o', taus, 2.1 * taus, '-o');
% grid on;
% legend('Явный', 'Неявный', 'Кранк-Никольсон', 'h^2');
% xlabel('tau');
% ylabel('Максимальная ошибка по всем точкам');

function res = U(x, t)
    res = exp(x) .* sin(x .* t);
end

function res_0 = U_0(x)
    res_0 = U(x, 0);
end

function res_f = F(x, t)
    res_f = exp(x) .* (cos(x .* t) .* (x - 2 * t) - sin(x .* t) .* (1 - t .^ 2));
end

function next_time = crank_nicolson_algo(xs, next_t, tau, delta, U_prev, F)
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
    VN = cat(2, repelem(h*h - 2*tau*(1-delta), x_len), ...
        repelem((1-delta)*tau, 2*(x_len-1)));
    C_r = sparse(IN, JN, VN);

    right_side = C_r * U_prev(2:x_len+1) + ...
        tau * h.^2 * F(xs(2:x_len+1), next_t-tau/2).';

    right_side(1) = right_side(1) + (1-delta)*tau*U_prev(1) + ...
        delta*tau*U(xs(1), next_t);

    mm = numel(right_side);
    right_side(mm) = right_side(mm) + (1-delta)*tau*U_prev(x_len+2) + ...
        delta*tau*U(xs(numel(xs)), next_t);

    nxt = sweep(A_l, B_l, C_l, right_side);
    next_time = cat(1, U(xs(1), next_t), nxt, U(xs(numel(xs)), next_t));
end
