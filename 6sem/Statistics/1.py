from functools import partial
from collections import OrderedDict

import numpy as np
from scipy import stats
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

samples_size = [10, 50, 1000]

class Sampler:
    def __init__(self, distribution, *args, **kwargs):
        self.distribution = distribution
        self.args = args
        self.kwargs = kwargs

    def __call__(self, size):
        return self.distribution.rvs(*self.args, **self.kwargs, size=size)


distributions = OrderedDict(
    norm=Sampler(stats.norm, 0, 1),
    cauchy=Sampler(stats.cauchy, 0, 1),
    laplace=Sampler(stats.laplace, 0, 1 / np.sqrt(2)),
    poisson=Sampler(stats.poisson, 10),
    uniform=Sampler(stats.uniform, -3, 6)
)


for sample_size in samples_size:
    df = pd.DataFrame(data=dict(
        zip(distributions.keys(), 
            map(lambda el: el(size=sample_size), distributions.values())
        )
    ))

    plt.figure()
    for i, column in enumerate(df):
        plt.subplot(3, 2, i+1)

        plt.title(column)
        hist, bins_ = np.histogram(df[column])
        freq = hist/np.sum(hist)
        plt.bar(bins_[:-1], freq, align="edge", width=np.diff(bins_), ec='black')

        dist = getattr(stats, column)

        if column != 'poisson':
            x = np.linspace(df[column].min(), df[column].max(), 100)
            y = dist.pdf(x, *distributions[column].args, **distributions[column].kwargs)
            plt.plot(x, y, color='red', linewidth=3)
        else:
            x = np.unique(df[column])
            y = dist.pmf(x, *distributions[column].args, **distributions[column].kwargs)
            plt.vlines(x, 0, y, color='red', linewidth=5)

    plt.suptitle(f'Sample size is {sample_size}')
    plt.show(block=False)

plt.waitforbuttonpress()