from functools import partial

import numpy as np
from scipy import stats
import pandas as pd
import matplotlib.pyplot as plt

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

    df.hist(weights=np.ones_like(df.index)/len(df.index), 
            ec='black', grid=False)
    plt.suptitle(f'Sample size is {sample_size}')
    plt.show(block=False)

plt.waitforbuttonpress()