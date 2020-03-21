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

    def __call__(self, size):
        return self.distribution.rvs(*self.args, **self.kwargs, size=size)

    def cdf(self, x):
        return self.distribution.cdf(x, *self.args, **self.kwargs)

        
sample_sizes = [20, 60, 100]
xlims = [[-4, 4]] * 4 + [[6, 14]]

distributions = OrderedDict(
    norm=Distribution(stats.norm, 0, 1),
    cauchy=Distribution(stats.cauchy, 0, 1),
    laplace=Distribution(stats.laplace, 0, 1 / np.sqrt(2)),
    uniform=Distribution(stats.uniform, -3, 6),
    poisson=Distribution(stats.poisson, 10),
)


for idx, (key, value) in enumerate(distributions.items()):
    plt.suptitle(f'Distibution: {key}. Cumulative distribtuion function')
    for i, sample_size in enumerate(sample_sizes):
        plt.subplot(1, 3, i+1)
        plt.title(f'Sample size: {sample_size}')
        x = value(size=sample_size)
        x = x[(xlims[idx][0] <= x) & (x <= xlims[idx][1])]
        plt.hist(x, cumulative=True, density=True, histtype='step', label='Empirical')
        x = np.linspace(xlims[idx][0], xlims[idx][1], num=500)
        plt.plot(x, value.cdf(x), label='True')
        plt.xlim(xlims[idx][0], xlims[idx][1])
        plt.legend()
    plt.show()