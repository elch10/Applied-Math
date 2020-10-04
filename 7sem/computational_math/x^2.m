% x^2 
x = 0.8;
a = 0;
b = 2;
error1 = [];
error2 = [];
error3 = [];
error4 = [];
steps = [];
h = 1e-16;
derivative1 = 2 * x;
derivative2 = 2;
while (h <= 1)
    app1 = ((x + h)^2 - x^2) / h;
    app2 = (x^2 - (x - h)^2) / h;
    app3 = ((x + h)^2 - (x - h)^2) / (2 * h);
    app4 = ((x + h).^2 - 2 * x.^2 + (x - h).^2 ) / h.^2;
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
title('Аппроксимация первой производной x^2')

figure
loglog(steps, error2)
hold on
grid on
loglog(steps, steps, 'p')
legend('Ошибка', 'h')
xlabel('Шаг')
ylabel('Ошибка')
title('Аппроксимация первой производной x^2')

figure
loglog(steps, error3)
hold on
grid on
loglog(steps, steps.^2, 'p')
legend('Ошибка', 'h^2')
xlabel('Шаг')
ylabel('Ошибка')
title('Аппроксимация первой производной x^2')

figure
loglog(steps, error4)
hold on
grid on
loglog(steps, steps.^2, 'p')
legend('Ошибка', 'h^2')
xlabel('Шаг')
ylabel('Ошибка')
title('Аппроксимация второй производной x^2')