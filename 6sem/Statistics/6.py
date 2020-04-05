import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

class SquaredLossRegression:
    """
    This class is used to 2 variables
    """
    def __init__(self):
        self.b0 = 0
        self.b1 = 0

    def fit(self, X, y):
        self.b1 = np.cov(X, y)[0, 1] / np.var(X)
        self.b0 = np.mean(y) - np.mean(X) * self.b1

    def predict(self, X):
        return self.b1 * X + self.b0

    def fit_predict(self, X, y):
        self.fit(X, y)
        return self.predict(X)

class RobustRegression(SquaredLossRegression):
    def fit(self, X, y):
        n = X.shape[0]
        r = np.mean(np.sign(X - np.median(X)) * np.sign(y - np.median(y)))
        l = n // 4 + (n % 4 != 0)
        j = n - l + 1
        q_x = (X[j] - X[l]) / 2
        q_y = (y[j] - y[l]) / 2
        self.b1 = r * q_y / q_x
        self.b0 = np.median(y) - np.median(X) * self.b1


sql = SquaredLossRegression()
rr = RobustRegression()

num = 20
x = np.linspace(-1.8, 2, num)
y = 2 + 2 * x + np.random.randn(num)

ax = plt.subplot(1, 2, 1)
ax.scatter(x, y, s=20)
pd.DataFrame(dict(LeastSquares=sql.fit_predict(x, y), 
                  LeastModules=rr.fit_predict(x, y), 
                  Real=(2 + 2 * x)), index=x)\
                  .plot(ax=ax, xlim=(-2, 2.2))

s = f'B_1: {sql.b1: .5f}. B_0: {sql.b0: .5f}. B_1R: {rr.b1: .5f}. B_0R: {rr.b0: .5f}'
plt.title(s)
print(s)

y[0] += 10
y[-1] -= 10
ax = plt.subplot(1, 2, 2)
ax.scatter(x, y, s=20)
pd.DataFrame(dict(LeastSquares=sql.fit_predict(x, y), 
                  LeastModules=rr.fit_predict(x, y), 
                  Real=(2 + 2 * x)), index=x)\
                  .plot(ax=ax, xlim=(-2, 2.2))
s = f'B_1: {sql.b1: .5f}. B_0: {sql.b0: .5f}. B_1R: {rr.b1: .5f}. B_0R: {rr.b0: .5f}'
plt.title(s)
print(s)
plt.show()
