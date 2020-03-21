from functools import partial
from collections import OrderedDict

import numpy as np
from scipy import stats
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

def silverman_rule(sample):
    n = sample.shape[0]
    std = np.std(sample)
    return 1.06 * std * n ** (-1 / 5)

class Distribution:
    def __init__(self, distribution, *args, **kwargs):
        self.distribution = distribution
        self.args = args
        self.kwargs = kwargs

    def __call__(self, size):
        return self.distribution.rvs(*self.args, **self.kwargs, size=size)

    def cdf(self, x):
        return self.distribution.cdf(x, *self.args, **self.kwargs)

    def pdf(self, x):
        if not isinstance(self.distribution, stats.rv_discrete):
            return self.distribution.pdf(x, *self.args, **self.kwargs)
        return self.distribution.pmf(x, *self.args, **self.kwargs)

        
sample_sizes = [20, 60, 100]
xlims = [[-4, 4]] * 4 + [[6, 14]]
bw_factors = [1/2, 1, 2]

distributions = OrderedDict(
    norm=Distribution(stats.norm, 0, 1),
    cauchy=Distribution(stats.cauchy, 0, 1),
    laplace=Distribution(stats.laplace, 0, 1 / np.sqrt(2)),
    uniform=Distribution(stats.uniform, -3, 6),
    poisson=Distribution(stats.poisson, 10),
)

for idx, (name, distr) in enumerate(distributions.items()):
    i = 1
    plt.suptitle(f'Distribution: {name}. Kernel density estimation')
    for size in sample_sizes:
        for factor in bw_factors:
            ax = plt.subplot(3, 3, i)

            plt.title(f'Sample size: {size}. Factor of H_n: {factor}')
            x = np.sort(distr(size=size))
            x = x[(xlims[idx][0] <= x) & (x <= xlims[idx][1])]
            sns.kdeplot(pd.Series(x, name='KDE'), bw=factor * silverman_rule(x), ax=ax)

            if name == 'poisson':
                x = np.arange(xlims[idx][0], xlims[idx][1]+1)
                plt.vlines(x, 0, distr.pdf(x), label='True')
            else:
                x = np.linspace(xlims[idx][0], xlims[idx][1])
                plt.plot(x, distr.pdf(x), label='True')
            
            plt.xlim(xlims[idx][0]-0.05, xlims[idx][1]+0.05)
            plt.legend()
            i += 1
    plt.show()