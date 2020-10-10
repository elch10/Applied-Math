function [Newdata, PCASpace, EigValues] = PCACov(data)
    [r,c]=size(data);
    m=mean(data, 1);

    % Subtract the mean [Centering the data]
    d=data-repmat(m,r,1);

    % Compute the covariance matrix (co)
    co=cov(d);

    % Compute the eigen values and eigen vectors of the covariance matrix
    [eigvector, d] = eig(co, 'vector');
    [EigValues, ind] = sort(d, 'descend');

    PCASpace=eigvector(:, ind);
    % Project the original data on the PCA space
    Newdata=data*PCASpace;
end

s = 'iris';

if isequal(s, 'modeling')
    % задаём параметры модельных данных
    m = [-1 0 1];
    co = [25 7.5 1.75;
           7.5 7 1.35;
           1.75 1.35 0.43];
    n = 500;
    
    % генерируем и рисуем выборку
    A = mvnrnd(m, co, n);
    clf;
    scatter3(A(:, 1), A(:, 2), A(:, 3));
    view(20, 15);
    labels = ones(n, 1);

elseif isequal(s, 'german')
    filename = 'german.data-numeric';
    data = importdata(filename, ' ', 0);
    p = 24;

    A = data(:, 1:p);
    labels = data(:, p+1);
    co = cov(A);

elseif isequal(s, 'iris')
    filename = 'iris.data';
    %data = importdata(filename, ',', 0);
    data = csvread(filename);
    p = 4;

    A = data(:, 1:p);
    labels = cat(1, ones([50, 1]), 2*ones([50, 1]), 3*ones([50, 1]));
    co = cov(A);
end

% используем pca
[z, PCASpace, eigvals] = PCACov(A);

format shortG;
disp('Собственные значения')
eigvals

disp('Информативность признаков')
I = eigvals / trace(co)

thresh = 0.9;
cumsum_I = cumsum(I);
disp('Кол-во признаков которые будут использоваться после уменьшения размерности');
n_idxs = find(cumsum_I > thresh)(1)

% и рисуем эти данные в проекции на новое пространство
scatter(z(:, 1), z(:, 2), 20, labels, 'filled');

%disp('Ковариации в новом пространстве:');
%cov(new_z)

