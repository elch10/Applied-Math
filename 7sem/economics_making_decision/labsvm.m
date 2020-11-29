data_name = 'wine';

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
    mu1 = [0 0];
    mu2 = [4 4];
    
    sigma1 = [1.5 0;
              0 1.5];
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
elseif isequal(data_name, 'герман')
    filename = 'german.data-numeric';
    data = importdata(filename, ' ', 0);
    p = 24;

    A = data(:, 1:p);
    labels = data(:, p+1);

    labels(labels == 2) = -1;
    co = cov(data);

    [z, PCASpace, eigvals] = PCACov(A);
    data = z(:, [1 2]);
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

kern_name = 'линейное ядро';
if isequal(kern_name, 'линейное ядро')
    kern = lin_kern;
elseif isequal(kern_name, 'полиномиальное ядро')
    kern = poly_kern;
elseif isequal(kern_name, 'гауссово ядро')
    kern = rbf_kern;
end


tresh = 0.001;
for c=[1 10 100]
    [alpha, sv_idxs, w, b] = SVM(data, labels, kern, c, tresh);
    pred_lbls = SVMpred(alpha, b, data, labels, data, kern);
    
    % scatter data and sv
    figure;
    scatter(data(:, 1), data(:, 2), 50, labels);
    title(strcat(data_name, '; ', kern_name, '; C=', int2str(c)));
    hold on;
    scatter(data(sv_idxs, 1), data(sv_idxs, 2), 'filled');

    % plot contour
    bandwidth = 2 / norm(w);
    xx = linspace(min(data(:, 1)), max(data(:, 1)), 100)';
    yy = linspace(min(data(:, 2)), max(data(:, 2)), 100)';
    [X,Y] = meshgrid(xx, yy);
    SS = cat(2, reshape(X, [numel(X), 1]), reshape(Y, [numel(Y), 1]));
    level = SVMpred(alpha, b, data, labels, SS, kern);
    contour_pred = reshape(level, [numel(xx), numel(yy)]);
    contour(X, Y, contour_pred, [-1, 0, 1], 'ShowText', 'on');

end