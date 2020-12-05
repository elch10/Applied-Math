data_name = 'герман pca';

if isequal(data_name, 'разделимые')
    mu1 = [0 0];
    mu2 = [5 5];
    
    sigma1 = [1 0;
              0 1];
    sigma2 = sigma1;

    n = 200;
    s1 = mvnrnd(mu1, sigma1, n);
    s2 = mvnrnd(mu2, sigma2, n);
    
    data = cat(1, s1, s2);
    labels = cat(1, -ones(n, 1), ones(n, 1));
elseif isequal(data_name, 'плохо разделимые')
    mu1 = [1 0];
    mu2 = [0 1];
    
    sigma1 = [1 0;
              0 1];
    sigma2 = sigma1;
    
    n = 200;
    s1 = mvnrnd(mu1, sigma1, n);
    s2 = mvnrnd(mu2, sigma2, n);
    
    data = cat(1, s1, s2);
    labels = cat(1, -ones(n, 1), ones(n, 1));
elseif isequal(data_name, 'xor')
    data = randn([100 2]);
    labels = cast(xor(data(:, 1)>0, data(:, 2)>0), 'double');
    labels(find(labels == 0)) = -1.0;
elseif isequal(data_name, 'кольца')
    r = sqrt(rand(100,1)); % Radius
    t = 2*pi*rand(100,1);  % Angle
    data1 = [r.*cos(t), r.*sin(t)]; % Points
    
    r2 = sqrt(3*rand(100,1)+1); % Radius
    t2 = 2*pi*rand(100,1);      % Angle
    data2 = [r2.*cos(t2), r2.*sin(t2)]; % points
    
    data = [data1;data2];
    labels = ones(200,1);
    labels(1:100) = -1;
elseif isequal(data_name, 'герман')
    filename = 'german.data-numeric';
    data = importdata(filename, ' ', 0);
    p = 24;

    A = normalize(data(:, 1:p), 1);
    labels = data(:, p+1);
    labels(labels == 2) = -1;
   
    data = A;
    cv = cvpartition(labels, 'HoldOut', 0.2);
    idx = cv.training;
    
    train_labels = labels(idx);
    test_labels = labels(~idx);

    train_data = data(idx, :);
    test_data = data(~idx, :);
elseif isequal(data_name, 'герман pca')
    filename = 'german.data-numeric';
    data = importdata(filename, ' ', 0);
    p = 24;

    A = normalize(data(:, 1:p), 1);
    labels = data(:, p+1);
    labels(labels == 2) = -1;

    [z, PCASpace, eigvals] = PCACov(A);
    data = z(:, [1 2]);
    
    cv = cvpartition(labels, 'HoldOut', 0.2);
    idx = cv.training;
    
    train_labels = labels(idx);
    test_labels = labels(~idx);

    train_data = data(idx, :);
    test_data = data(~idx, :);
elseif isequal(data_name, 'wine')
    data = importdata('wine.data', ',', 0);

    labels = data(:, 1);
    data = data(:, 2:end);
    idxs = find(labels == 1 | labels == 2);
    labels = labels(idxs);
    data = data(idxs, :);
    
    mn = mean(data, 1);
    sdev = std(data);
    data = (data - repmat(mn, [numel(labels) 1])) ./ (repmat(sdev, [numel(labels) 1]));
    labels(labels == 2) = -1;
    
    [z, PCASpace, eigvals] = PCACov(data);
    data = z(:, [1 2]);
end


poly_kern = @(Xtrain, Xpred) (1 + Xtrain * Xpred').^2;
lin_kern = @(Xtrain, Xpred) Xtrain * Xpred';
rbf_kern = @(Xtrain, Xpred) exp(-pdist2(Xtrain, Xpred) / 2);

% Choosing kernel affects only to 'not german' data
% For german data there is auto search
kern_name = 'linear';
if isequal(kern_name, 'linear')
    kern = lin_kern;
elseif isequal(kern_name, 'polynomial')
    kern = poly_kern;
elseif isequal(kern_name, 'gaussian')
    kern = rbf_kern;
end

tresh = 0.001;
if isequal(data_name, 'герман') | isequal(data_name, 'герман pca')
    model = fitcsvm(train_data, train_labels, ...
        'OptimizeHyperparameters', 'all', ...
        'HyperparameterOptimizationOptions',struct('Kfold',5, ...
        'MaxObjectiveEvaluations', 10, 'ShowPlots', false));
    model
    test_pred = predict(model, test_data);
    figure;
    confusionchart(confusionmat(test_labels, test_pred), [-1 1]);
else
    for c=[1 5 10]
        [alpha, sv_idxs, w, b] = SVM(data, labels, kern, c, tresh);
        pred_lbls = SVMpred(alpha, b, data, labels, data, kern);
        bandwidth = 2 / norm(w)

        % scatter data and sv
        figure;
        gscatter(data(:, 1), data(:, 2), labels, 'rb', 'x');
        title(strcat(data_name, '; ', kern_name, '; C=', int2str(c)));
        hold on;
        gscatter(data(sv_idxs, 1), data(sv_idxs, 2), labels(sv_idxs), 'rb', 'o');
    
        % plot contour
        xx = linspace(min(data(:, 1)), max(data(:, 1)), 100)';
        yy = linspace(min(data(:, 2)), max(data(:, 2)), 100)';
        [X,Y] = meshgrid(xx, yy);
        SS = cat(2, reshape(X, [numel(X), 1]), reshape(Y, [numel(Y), 1]));
        level = SVMpred(alpha, b, data, labels, SS, kern);
        contour_pred = reshape(level, [numel(xx), numel(yy)]);
        contour(X, Y, contour_pred, [-1, 0, 1], 'ShowText', 'on'); 
    end
end
