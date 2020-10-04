% sin(x)
x = 0.8;
a = 0;
b = pi;
error1 = [];
error2 = [];
error3 = [];
error4 = [];
steps = [];
h = 1e-16;
derivative1 = cos(x);
derivative2 = -sin(x);
while (h <= 1)
    app1 = (sin(x + h) - sin(x)) / h;
    app2 = (sin(x) - sin(x - h)) / h;
    app3 = (sin(x + h) - sin(x - h)) / (2 * h);
    app4 = (sin(x + h) - 2 * sin(x) + sin(x - h)) / h^2;
    er1 = abs(derivative1 - app1);
    er2 = abs(derivative1 - app2);
    er3 = abs(derivative1 - app3);
    er4 = abs(derivative2 - app4);
    error1 = [error1 er1];
    error2 = [error2 er2];
    error3 = [error3 er3];
    error4 = [error4 er4];
    steps = [steps h];
    h = h * 10;
end
figure
loglog(steps, error1)
hold on
grid on
loglog(steps, steps, 'p')
legend('Ошибка', 'h')
xlabel('Шаг')
ylabel('Ошибка')
title('Аппроксимация первой производной sin(x)')

figure
loglog(steps, error2)
hold on
grid on
loglog(steps, steps, 'p')
legend('Ошибка', 'h')
xlabel('Шаг')
ylabel('Ошибка')
title('Аппроксимация первой производной sin(x)')

figure
loglog(steps, error3)
hold on
grid on
loglog(steps, steps.^2, 'p')
legend('Ошибка', 'h^2')
xlabel('Шаг')
ylabel('Ошибка')
title('Аппроксимация первой производной sin(x)')


figure
loglog(steps, error4)
hold on
grid on
loglog(steps, steps.^2, 'r')
legend('Ошибка', 'h^2')
xlabel('Шаг')
ylabel('Ошибка')
title('Аппроксимация второй производной sin(x)')