r = 0.05;
sig = 0.2;
E = 140;
T = 1;
RIGHT_BOUND = 500;

errors = [];
steps_tau = [0.0005, 0.001, 0.01, 0.1];
% steps_tau = [0.01];

type = 'PUT';
for step_tau=steps_tau
    step_tau
    tau = 0:step_tau:T;
    step_s = 100 * step_tau;
    S = 0:step_s:RIGHT_BOUND;
    price = optionPriceImplicit(E,r,sig,S,tau,type);

    [XX, YY] = meshgrid(tau, S);
    d1 = @(S, tau) (log(S/E) + (r + sig*sig/2) * (T-tau)) ./ (sig * sqrt(T-tau));
    d2 = @(S, tau) (log(S/E) + (r - sig*sig/2) * (T-tau)) ./ (sig * sqrt(T-tau));
    if isequal(type, 'CALL')
        multip = 1;
    else
        multip = -1;
    end
    RealPrice = multip * YY .* normcdf(multip * d1(YY, XX)) - ...
        multip * E .* exp(-r * (T-XX)) .* normcdf(multip * d2(YY, XX));
    
    figure;
    surf(tau, S, price);
    title(strcat('Предсказанное - tau=', num2str(step_tau)));
    
    figure;
    surf(tau, S, RealPrice);
    title(strcat('Реальное - tau=', num2str(step_tau)));
    errors = [errors nanmax(nanmax(abs(RealPrice - price)))];
end

format shortG;
figure;
loglog(steps_tau, errors);
title('Неявный');
xlabel('tau');
ylabel('Максимальная ошибка по всем внутренним точкам');
