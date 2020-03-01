from functools import partial

import numpy as np
from scipy import stats
import pandas as pd
import matplotlib.pyplot as plt


def quartile_half_sum(col):
    a, b = np.quantile(col.values, [0.25, 0.75], interpolation='higher')
    return (a + b) / 2

def reduced_mean(col):
    r = col.shape[0] // 4
    return np.mean(col[r:-r])

def extreme_half_sum(col):
    return (col.min() + col.max()) / 2

samples_size = [10, 50, 100]
distributions = dict(
    normal=partial(stats.norm.rvs, 0, 1),
    cauchy=partial(stats.cauchy.rvs, 0, 1),
    laplace=partial(stats.laplace.rvs, 0, 1 / np.sqrt(2)),
    poisson=partial(stats.poisson.rvs, 10),
    uniform=partial(stats.uniform.rvs, -3, 6)
)



for sample_size in samples_size:
    df = pd.DataFrame(data=dict(
        zip(distributions.keys(), 
            map(lambda el: el(size=sample_size), distributions.values())
        )
    ))

    print(df.agg([np.mean, np.median, extreme_half_sum,
                  reduced_mean, quartile_half_sum], axis=0))
    print()

