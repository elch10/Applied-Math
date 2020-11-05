mu1 = [0 0];
mu2 = [10 10];

sigma1 = [1 0;
          0 1];
sigma2 = sigma1;

n = 100;
s1 = mvnrnd(mu1, sigma1, n);
s2 = mvnrnd(mu2, sigma2, n);

s = cat(1, s1, s2);
labels = cat(1, -ones(n, 1), ones(n, 1));


% This Kernel is the Vapnik's polynomial,
poly_kern = @(Xtrain, Xpred) (1 + Xtrain * Xpred').^2;
lin_kern = @(Xtrain, Xpred) Xtrain * Xpred';
rbf_kern = @(Xtrain, Xpred) pdist2(Xtrain, Xpred) / 2;

kern = lin_kern;
[alpha, sv_idxs, w, b] = SVM(s, labels, kern, 1, 0.001);
pred_lbls = SVMpred(alpha, b, s, labels, s, kern);

% scatter all and sv
figure;
scatter(s(:, 1), s(:, 2), 50, labels);
hold on;
scatter(s(sv_idxs, 1), s(sv_idxs, 2), 'filled');


% plot contour
bandwidth = 2 / norm(w);
xx = linspace(min(s(:, 1)), max(s(:, 1)), 100)';
yy = linspace(min(s(:, 2)), max(s(:, 2)), 100)';
[X,Y] = meshgrid(xx, yy);
SS = cat(2, reshape(X, [numel(X), 1]), reshape(Y, [numel(Y), 1]));
level = SVMpred(alpha, b, s, labels, SS, kern);
contour_pred = reshape(level, [numel(xx), numel(yy)]);
contour(X, Y, contour_pred, [-1, 0, 1], 'ShowText', 'on');
