function [myavg, CI] = avg_ci(X)
myavg = sum(X)/size(X, 1);
CI = 1.96.*(std(X)./sqrt(size(X, 1)));
%1.64 - 90%
%1.96 - 95%
%2.33 - 98%
%2.58 - 99%
end