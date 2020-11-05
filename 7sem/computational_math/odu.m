a = 0;
b = 1;
fix = 0.8;
h = 1e-7;
error = [];
steps = [];
max_error = [];

while (h < 1)
    h
    x = double(a + h);
    n = int64((b-a)/h);

    right = zeros(n-1, 1);
    right(1, 1) = 2*h.^2*exp(x)*cos(x) - sin(a)*exp(a);
    i = 2;
    x = double(a + i*h);
    while (i < n-1)
        right(i, 1) = h.^2 * 2 * exp(x) * cos(x);
        i = i + 1;
        x = x + h;
    end
    x = double(a + (n-1)*h);
    right(n-1, 1) = 2*h.^2*exp(x)*cos(x) - exp(b)*sin(b);

    A = ones(n-2, 1);
    B = -2 * ones(n-1, 1);
    C = ones(n-2, 1);

    uu = sweep(A, B, C, right);
    u = cat(1, exp(a)*sin(a), uu, exp(b)*sin(b));

    u_true = sin(fix) * exp(fix);
    i_false = int64((fix - a) / h) + 1;
    er = abs(u_true - u(i_false));
    error = [error er];

    xs = linspace(a+h, b-h, n-1);
    er = max(abs(sin(xs).*exp(xs)-uu'));
    max_error = [max_error er];
    steps = [steps h];
    h = h * 10;
end

figure;
loglog(steps, error)
hold on;
grid on;
loglog(steps, max_error)
loglog(steps, steps.^2)
legend('Ошибка в фиксированной точке', 'Максимальная ошибка по всем точкам', 'h^2', 'Location', 'southeast')
xlabel('Шаг')
ylabel('Ошибка')

