run_a = false

% To run a) uncomment this section and comment next comment
% ---------------------------------------------------------
if run_a
    mu1 = [-2 -1 0];
    mu2 = [1 2 3];
    cov_matr = [1 0 0;
                0 4 0;
                0 0 9];
           
    n1 = 200;
    n2 = 800;
    OB1 = mvnrnd(mu1, cov_matr, n1);
    OB2 = mvnrnd(mu2, cov_matr, n2);
    q1 = n1 / (n1 + n2);
    q2 = n2 / (n1 + n2);
    p = 3;
    test_n = 100;
    N = 2 * test_n;
    test_part1 = mvnrnd(mu1, cov_matr, test_n);
    test_part2 = mvnrnd(mu2, cov_matr, test_n);
    test_selection = cat(1, test_part1, test_part2);
    
    hold on
    scatter3(OB1(:, 1), OB1(:, 2), OB1(:, 3))
    scatter3(OB2(:, 1), OB2(:, 2), OB2(:, 3))
    hold off
    view(60, 15)
else
    filename = 'german.data-numeric';
    delimiterIn = ' ';
    headerlinesIn = 0;
    A = importdata(filename, delimiterIn, headerlinesIn);
    p = 24;
    
    hpartition = cvpartition(A(:, 25),'Holdout',0.3);
    idxTrain = training(hpartition);
    tblTrain = A(idxTrain,:);
    idxTest = test(hpartition);
    test_selection = A(idxTest,:);
    
    OB1 = tblTrain(tblTrain(:, 25) == 1, :);
    OB2 = tblTrain(tblTrain(:, 25) == 2, :);
    
    n1 = size(OB1, 1);
    n2 = size(OB2, 1);
    N = size(test_selection, 1);
    q1 = n1 / (n1 + n2);
    q2 = n2 / (n1 + n2);
end

mu_hat1 = mean(OB1(:, 1:p), 1);
mu_hat2 = mean(OB2(:, 1:p), 1);

S1 = cov(OB1(:, 1:p));
S2 = cov(OB2(:, 1:p));
S = ((n1 - 1) * S1 + (n2 - 1) * S2) / (n1 + n2 - 2);

a = inv(S) * (mu_hat1 - mu_hat2).';

z1 = OB1(:, 1:p) * a;
z2 = OB2(:, 1:p) * a;

z_hat1 = mean(z1);
z_hat2 = mean(z2);

var_z = a.' * S * a;

right_side = (z_hat1 + z_hat2) / 2 + log(q2 / q1);

disp('Смещенная оценка расстояния Махаланобиса:');
biased_estimate_of_Mahalanobis_distance = (z_hat1 - z_hat2).^2 / var_z;
disp(biased_estimate_of_Mahalanobis_distance);
disp('Несмещенная оценка расстояния Махаланобиса:');
unbiased_estimate_of_Mahalanobis_distance = (n1 + n2 - p - 3) * biased_estimate_of_Mahalanobis_distance /(n1 + n2 - 2) - p * ((1 / n1) + (1 / n2));
disp(unbiased_estimate_of_Mahalanobis_distance);

OB = cat(1, OB1, OB2);
results = OB(:, 1:p) * a >= right_side;

matr = zeros(2, 2);

matr(1, 1) = (results == 1).' * (1:size(results, 1) <= n1).';
matr(1, 2) = (results == 0).' * (1:size(results, 1) <= n1).';
matr(2, 1) = (results == 1).' * (1:size(results, 1) > n1).';
matr(2, 2) = (results == 0).' * (1:size(results, 1) > n1).';

disp('Результаты классификации ОВ1 и ОВ2:');
matr

disp('Вероятность ошибки на обучающей выборке полученная из таблицы:');
disp('P(2|1)=');
P_hat21 = matr(1, 2)  / (matr(1, 1)  + matr(1, 2) );
disp(P_hat21);
disp('P(1|2)=')
P_hat12 = matr(2, 1)  / (matr(2, 2) + matr(2, 1));
disp(P_hat12)

K = log(q2 / q1);
P_hat21 = normcdf( (( K - unbiased_estimate_of_Mahalanobis_distance / 2 ) / sqrt(unbiased_estimate_of_Mahalanobis_distance)) , 0 , 1 );
P_hat12 = normcdf( ((-K - unbiased_estimate_of_Mahalanobis_distance / 2) / sqrt(unbiased_estimate_of_Mahalanobis_distance)), 0 , 1);

disp('Оценка вероятности ошибочной классификации вычисленная по формуле (*):')
disp('P(2|1)=')
disp(P_hat21);
disp('P(1|2)=')
disp(P_hat12)

matr = zeros(2, 2);
# if it is more or equal (true), then it is first class
results = test_selection(:, 1:p) * a >= right_side;
if run_a
    % ---------------------------------------------------------
    % a)
    matr(1, 1) = (results == 1).' * (1:size(results, 1) <= test_n).';
    matr(1, 2) = (results == 0).' * (1:size(results, 1) <= test_n).';
    matr(2, 1) = (results == 1).' * (1:size(results, 1) > test_n).';
    matr(2, 2) = (results == 0).' * (1:size(results, 1) > test_n).';
else
    % ---------------------------------------------------------
    % b)
    matr(1, 1) = (results == 1).' * (test_selection(:, p+1) == 1);
    matr(1, 2) = (results == 0).' * (test_selection(:, p+1) == 1);
    matr(2, 1) = (results == 1).' * (test_selection(:, p+1) == 2);
    matr(2, 2) = (results == 0).' * (test_selection(:, p+1) == 2);
    % ---------------------------------------------------------
end

disp('Результаты классификации тестовой выборки:');
matr

disp('Вероятность ошибки на тестовой выборке полученная из таблицы:');
disp('P(2|1)=');
P_hat21 = matr(1, 2)  / (matr(1, 1)  + matr(1, 2) );
disp(P_hat21);
disp('P(1|2)=')
P_hat12 = matr(2, 1)  / (matr(2, 2) + matr(2, 1));
disp(P_hat12)


[row, col] = find(triu(corr(A(:, 1:p))) > 0.5); 
indices = find(row != col)

