function [alpha, sv_idxs, w, b] = SVM(XTrain, YTrain, kern, C, threshold)
N = size(XTrain,1);

% Defining variables for quadprog
Dy = diag(YTrain);
K = kern(XTrain, XTrain);
H = (Dy * K * Dy) / 2;

f = -ones(N,1);
A = [];
c = [];
Aeq = YTrain'; 
ceq = 0;
cl = zeros(N,1);
cu = C * ones(N,1);

alpha = quadprog(H,f,A,c,Aeq,ceq,cl,cu);
alpha(find(alpha < threshold)) = 0;
alpha(find(alpha > C - threshold)) = 0;

sv_idxs = find(alpha > threshold & alpha < C - threshold);

w = (alpha .* YTrain)' * XTrain;
% d = 1 ./ YTrain(sv_idxs, :)' - (w * XTrain(sv_idxs, :)');
d = 1 ./ YTrain(sv_idxs, :)' - SVMpred(alpha, 0, XTrain, ...
    YTrain, XTrain(sv_idxs, :), kern);
b = mean(d);
end

