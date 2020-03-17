from functools import partial
from collections import OrderedDict

import numpy as np
from scipy import stats
import pandas as pd
import matplotlib.pyplot as plt

class Distribution:
    def __init__(self, distribution, *args, **kwargs):
        self.distribution = distribution
        self.args = args
        self.kwargs = kwargs
        self.Q1, self.Q3 = self.distribution.ppf([0.25, 0.75], *self.args, **self.kwargs)
        l, r = self.Q1, self.Q3
        X1 = l - 1.5 * (r - l)
        X3 = r + 1.5 * (r - l)
        self.X1, self.X3 = X1, X3
        self.top = self.distribution.cdf(X1, *self.args, **self.kwargs)\
                   + self.distribution.sf(X3, *self.args, **self.kwargs)

    def __call__(self, size):
        return self.distribution.rvs(*self.args, **self.kwargs, size=size)

    def outliers_probability(self, rvs):
        return ((rvs < self.X1) | (rvs > self.X3)).sum() / rvs.shape[0]
        
samples_size = [20, 100]
distributions = OrderedDict(
    norm=Distribution(stats.norm, 0, 1),
    cauchy=Distribution(stats.cauchy, 0, 1),
    laplace=Distribution(stats.laplace, 0, 1 / np.sqrt(2)),
    poisson=Distribution(stats.poisson, 10),
    uniform=Distribution(stats.uniform, -3, 6)
)

repeats = 1000

for sample_size in samples_size:
    print(f'Sample size: {sample_size}')
    results = np.zeros((repeats,len(distributions)))
    for i in range(repeats):
        df = pd.DataFrame(data=dict(
            zip(distributions.keys(), 
                map(lambda el: el(size=sample_size), distributions.values())
            )
        ))
        for j, column in enumerate(df):
            results[i, j] = distributions[column].outliers_probability(df[column])
    df.plot.box()
    plt.title(f'Sample size: {sample_size}')
    plt.show(block=False)
    results = np.mean(results, axis=0)
    top = [distr.top for distr in distributions.values()]
    q1 = [distr.Q1 for distr in distributions.values()]
    q3 = [distr.Q3 for distr in distributions.values()]
    x1 = [distr.X1 for distr in distributions.values()]
    x3 = [distr.X3 for distr in distributions.values()]
    print(pd.DataFrame(dict(Q1=q1, Q3=q3, X1=x1, X3=x3, TOP=top, results=results), index=distributions.keys()))
    print()
plt.waitforbuttonpress()