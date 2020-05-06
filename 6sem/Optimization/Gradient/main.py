import numpy as np
import matplotlib.pyplot as plt


a, b, c = 3, 3, 1

def norm(x):
    return np.sum(x ** 2)

def f(x1, x2):
    return (x1**2) + a*(x2**2) + np.sin(b*x1+c*x2) + 3*x1 + 2*x2

def grad_f(x1, x2):
    grad_x1 = b*np.cos(b*x1+c*x2) + 2*x1 + 3
    grad_x2 = 2*a*x2 + c*np.cos(b*x1+c*x2) + 2
    return np.array([grad_x1, grad_x2])

def hessian(x1, x2):
    res = np.array([
        [2 - np.sin(b*x1+c*x2)*(b**2), -b*c*np.sin(b*x1+c*x2)],
        [-b*c*np.sin(b*x1+c*x2), 2*a - np.sin(b*x1+c*x2)*(c**2)]
    ])
    return res

def hessian_inverse(x1, x2):
    det = 4*a - 2*(a*(b**2)+c**2)*np.sin(b*x1+c*x2)
    hess = hessian(x1, x2)
    hess[0, 0], hess[1, 1] = -hess[1, 1], -hess[0, 0]
    return hess / -det

def gradient_descent(x0, eps):
    alpha = 0.1
    print(f'Gradient descent. Initial point: {x0}. Eps: {eps}. Step: {alpha}')
    step = 0
    while True:
        grad = grad_f(*x0)
        n = norm(grad)
        if n < eps:
            break
        x0 -= alpha * grad
        step += 1
        print(f'Iteration: {step} - {x0}')
    return x0

def second_order_descent(x0, eps):
    print(f'Second order descend. Initial point: {x0}. Eps: {eps}')
    alpha = 1
    step = 0
    while True:
        grad = grad_f(*x0)
        n = norm(grad)
        if n < eps:
            break
        d = hessian_inverse(*x0) @ grad
        while f(*(x0 - alpha * d)) > f(*x0) - eps * alpha * np.dot(grad, d):
            alpha /= 1.5
            if np.isclose(alpha, 0):
                print('Newton method didnt converge!!!')
                return x0
        x0 -= alpha * d
        step += 1
        print(f'Iteration: {step} - {x0}')
    return x0

def applicability():
    y = np.linspace(-1, 1, 100)
    x = np.linspace(-5, 1, 100)
    xy = np.c_[x, y]
    values = []
    for i in xy:
        for j in xy:
            values.append((j @ (hessian(*i) @ j)) / norm(j))
    return np.min(values), np.max(values)

epss = [0.1, 0.001, 0.0001]
x0 = [-2, 5]
for eps in epss:
    ans = gradient_descent(x0, eps)
    print(f'Ans: {ans}\n')

    ans = second_order_descent(x0, eps)
    print(f'Ans: {ans}\n')

# x = np.linspace(-4, 1, 100)
# y = np.linspace(-1, 1, 100)
# xx, yy = np.meshgrid(x, y)
# plt.contourf(xx, yy, f(xx, yy))
# plt.colorbar()
# plt.show()

# print(applicability())