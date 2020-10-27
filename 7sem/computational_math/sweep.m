function res = sweep(a, b, c, f)
    a = cat(1, 0, a);
    n = size(b, 1);

    B = ones(n, 1);
    B(1, 1) = b(1, 1);

    F = ones(n, 1);
    F(1, 1) = f(1, 1);
    for i=2:n
        B(i, 1) = b(i, 1) - a(i, 1) * c(i-1, 1) / B(i-1, 1);
        F(i, 1) = f(i, 1) - a(i, 1) * F(i-1, 1) / B(i-1, 1);
    end
    
    x = zeros(n, 1);
    x(n, 1) = F(n, 1) / B(n, 1);
    for i=n-1:-1:1
        x(i, 1) = (F(i, 1) - c(i, 1)*x(i+1, 1)) / B(i, 1);
    end
    res = x;
end