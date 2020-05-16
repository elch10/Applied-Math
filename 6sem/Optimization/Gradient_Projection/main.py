import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

class GradientProjectorOptimizer:
    def __init__(self, f, grad_f, C, d, F, g, step=1, reduction=.5):
        """
        Solves minimization problem with gradient projection
        min f(x) w.s.t
        Cx=d, Fx<=g
        """
        assert C.shape[0] == d.shape[0]
        assert F.shape[0] == g.shape[0]
        assert C.shape[1] == F.shape[1]

        self.f = f
        self.grad_f = grad_f
        self.C = C
        self.d = d
        self.F = F
        self.g = g
        self.step = step
        self.reduction = reduction

    def _extract_constraints(self):
        mm = self.F @ self.x
        self.active_idx = np.nonzero(np.isclose(mm, self.g))[0]
        self.passive_idx = np.nonzero(~np.isclose(mm, self.g))[0]

    def _remove_rows(self, A):
        w = -np.linalg.inv(A @ A.T) @ A @ self.grad_f(self.x)
        u = w[self.C.shape[0] + self.active_idx]
        positive = u >= 0

        if np.all(positive):
            return False

        self.active_idx = self.active_idx[positive]
        return True

    def _step(self, s):
        alpha = self.step
        while not np.isclose(alpha, 0, 1e-7):
            next_point = self.x + alpha * s
            if np.all(self.F[self.passive_idx] @ next_point <= self.g[self.passive_idx])\
                and self.f(next_point) < self.f(self.x):
                break
            alpha *= self.reduction
        
        if np.isclose(alpha, 0, 1e-7):
            return False
        
        self.x = self.x + alpha * s
        return True


    def optimize(self, x0, verbose=1):
        assert np.all(np.isclose(self.C @ x0, self.d))

        self.x = x0
        self._extract_constraints()
        n = self.x.shape[0]

        while True:
            A = np.r_[self.C, self.F[self.active_idx]]
            if A.shape[0] == 0 and np.all(F @ self.x < g):
                P = np.eye(n)
            else:
                P = np.eye(n) - A.T @ np.linalg.inv(A @ A.T) @ A
            
            s = -P @ self.grad_f(self.x)
            if np.all(np.isclose(s, 0)):
                if A.shape[0] == 0 or not self._remove_rows(A):
                    break
            else:
                if not self._step(s):
                    break
            
            if verbose:
                print(self.x)
                
        return self.x

def f(x):
    # 2x^2 + 2y^2 + 3z^2 + w^2 + 4x - 6w
    return 2*x[0]**2 + 2*x[1]**2 + 3*x[2]**2 + x[3]**2 + 4*x[0] - 6*x[3]

def grad_f(x):
    return np.array([
        4*x[0] + 4,
        4*x[1],
        6*x[2],
        2*x[3] - 6,
    ])


if __name__ == "__main__":
    C = np.array([
        [1, 0, 0, 1], # x + w = 2
        [0, 0, 1, 0]  # z = 0
    ])
    d = np.array([2, 0])
    F = np.array([
        [1, 1, 1, 0], # x + y + z <= 5
        [0, 0, 0, -1] # w >= 0
    ])
    g = np.array([5, 0])
    optim = GradientProjectorOptimizer(f, grad_f, C, d, F, g)

    x_star = optim.optimize(np.array([-8, 4, 0, 10]))
    print(f'Ans: {x_star}')