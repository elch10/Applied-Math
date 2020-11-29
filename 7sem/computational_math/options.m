r = 0.05;
sig = 0.2;
E = 140;
T = 1;
RIGHT_BOUND = 500;

errors = [];
steps_tau = logspace(-2, 0, 3);

for step_tau=steps_tau
    step_tau
    tau = 0:step_tau:T;
    step_s = 10 * step_tau;
%     step_s = 1;
    S = 0:step_s:RIGHT_BOUND;
    price = optionPriceImplicit(E,r,sig,S,tau,'CALL');

    [XX, YY] = meshgrid(tau, S);
    d1 = @(S, tau) (log(S/E) + (r + sig*sig/2) * (T-tau)) ./ (sig * sqrt(T-tau));
    d2 = @(S, tau) (log(S/E) + (r - sig*sig/2) * (T-tau)) ./ (sig * sqrt(T-tau));
    RealPrice = YY .* normcdf(d1(YY, XX)) - E .* exp(-r * (T-XX)) .* normcdf(d2(YY, XX));
    
    figure;
    surf(tau, S, price);
    title(strcat('Предсказанное - tau=', num2str(step_tau)));
    
    figure;
    surf(tau, S, RealPrice);
    title(strcat('Реальное - tau=', num2str(step_tau)));
%     nanmax(nanmax(abs(RealPrice - price)))
    errors = [errors nanmax(nanmax(abs(RealPrice - price)))];
end

format shortG;
figure;
loglog(steps_tau, errors);
% legend('Неявный метод с шагом по S=10*tau');
title('Неявный');
xlabel('tau');
ylabel('Максимальная ошибка по всем внутренним точкам');
