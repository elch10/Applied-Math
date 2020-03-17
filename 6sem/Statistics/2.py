from functools import partial
from collections import OrderedDict

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

samples_size = [10, 50, 1000]
distributions = OrderedDict(
    normal=partial(stats.norm.rvs, 0, 1),
    cauchy=partial(stats.cauchy.rvs, 0, 1),
    laplace=partial(stats.laplace.rvs, 0, 1 / np.sqrt(2)),
    poisson=partial(stats.poisson.rvs, 10),
    uniform=partial(stats.uniform.rvs, -3, 6)
)

repeats = 1000
agg_funcs = [np.mean, np.median, extreme_half_sum,
             reduced_mean, quartile_half_sum]

for sample_size in samples_size:
    print(f'Sample size: {sample_size}')
    results = []
    for i in range(repeats):
        df = pd.DataFrame(data=dict(
            zip(distributions.keys(), 
                map(lambda el: el(size=sample_size), distributions.values())
            )
        ))
        results.append(df.agg(agg_funcs, axis=0).values)
    
    results = np.array(results)
    mean = np.mean(results, axis=0)
    var = np.var(results, axis=0)
    result = np.zeros((mean.shape[0], mean.shape[1]*2))
    for i in range(mean.shape[1]):
        result[:, 2*i] = mean[:, i]
        result[:, 2*i+1] = var[:, i]
    result = pd.DataFrame(result, index=['Среднее', 'Медиана', 'Полусумма экстремальных', 'Полусумма квартилей', 'Усечённое среднее'],
                          columns=pd.MultiIndex.from_product([distributions.keys(), ['mean', 'var']]))
    result.to_csv(f'sample_{sample_size}.csv')