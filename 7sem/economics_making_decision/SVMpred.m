function classes = SVMpred(alpha, b, XTrain, YTrain, XPred, kern)
   res = (alpha .* YTrain)' * kern(XTrain, XPred);
   classes = res + b;
end