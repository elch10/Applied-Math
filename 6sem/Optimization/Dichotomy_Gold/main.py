import math

gr = (math.sqrt(5) + 1) / 2

def gss(f, a, b, tol=1e-5):
    """Golden section search
    to find the minimum of f on [a,b]
    f: a strictly unimodal function on [a,b]

    Example:
    >>> f = lambda x: (x-2)**2
    >>> x = gss(f, 1, 5)
    >>> print("%.15f" % x)
    2.000009644875678

    """
    c = b - (b - a) / gr
    d = a + (b - a) / gr
    while abs(c - d) > tol:
        if f(c) < f(d):
            b = d
        else:
            a = c

        # We recompute both c and d here to avoid loss of precision which may lead to incorrect results or infinite loop
        c = b - (b - a) / gr
        d = a + (b - a) / gr
        print(f'left: {c:.3f}, right: {d:.3f}')

    return (b + a) / 2

def half_division_method(f, a, b, tol=1e-6):
    x = (a + b) / 2
    while abs(f(x)) >= tol:
        x = (a + b) / 2
        a, b = (a, x) if f(a) * f(x) < 0 else (x, b)
        print(f'left: {a:.3f}, right: {b:.3f}')
    return (a + b) / 2

def counted(f):
    def wrapped(*args, **kwargs):
        wrapped.calls += 1
        return f(*args, **kwargs)
    wrapped.calls = 0
    return wrapped

@counted
def f(x):
    return x**3 - 8


if __name__ == '__main__':
    ans = half_division_method(f, -20, 20, 1e-3)
    print(f'Half division method ans: {ans}. F was called {f.calls} times')

    f.calls = 0
    ans = gss(f, -20, 20, 1e-3)
    print(f'Golden section search ans: {ans}. F was called {f.calls} times')
