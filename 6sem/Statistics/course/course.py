import sys, os
from functools import wraps
from time import time

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
from fastdtw import fastdtw

import pyglobus
from sawtooth_frequency_computation import get_valid_freq

def timing(f):
    @wraps(f)
    def wrap(*args, **kw):
        ts = time()
        result = f(*args, **kw)
        te = time()
        print('func:%r took: %2.4f sec' % \
          (f.__name__,  te-ts))
        return result
    return wrap


def normalize_signal(signal):
    return (signal - np.mean(signal)) / np.std(signal)

@timing
def dtw_dist(sig1, sig2, radius=200):
    # print('Computing...')
    sig1 = normalize_signal(sig1)
    sig2 = normalize_signal(sig2)
    dist, _ = fastdtw(sig1, sig2, radius=radius)
    return dist

def normalized_similarity(dtw_value, mx):
    return (mx - dtw_value) / mx

def matrix_compare(signals, dist_func):
    """
    dist_func must takes 2 signals and returns the distance
    """
    l = len(signals)

    pairs = []
    mn_lens = []
    for i in range(l):
        for j in range(i+1, l):
            pairs.append([signals[i], signals[j]])
            mn_lens.append(min(len(signals[i]), len(signals[j])))

    from multiprocessing import Pool
    with Pool(8) as p:
        dtw = p.starmap(dtw_dist, pairs)

    normalized_sim = []
    for l, d in zip(mn_lens, dtw):
        normalized_sim.append(normalized_similarity(d, l))

    matrix = np.ones((len(signals), len(signals)))
    indices = np.triu_indices(len(signals), k=1)
    matrix[indices] = normalized_sim
    matrix[indices[1], indices[0]] = normalized_sim

    return matrix

def read_signals(sht_files, signals_n):
    x, y = [], []
    for file, n in zip(sht_files, signals_n):
        signal = pyglobus.util.ShtReader(file).get_signal(n)
        x.append(signal.get_data_x())
        y.append(signal.get_data_y())
    return x, y

# Extracting ROI from signal
def get_roi_idx(y, mean_scale=0.96):
    threshold = np.mean(y) * mean_scale

    start_index, end_index = 0, 0
    data_length = y.shape[0]

    start_index = np.where(y > threshold)[0][0]
    end_index = data_length - np.where(y[::-1] > threshold)[0][0]

    return start_index, end_index

def get_roi(x, y):
    s, e = get_roi_idx(y)
    return x[s:e], y[s:e]

def files_dtw(sht_files, signals_n, dist_func):
    x, y = read_signals(sht_files, signals_n)

    for i in range(len(x)):
        x[i], y[i] = get_roi(x[i], y[i])
        # print(f'Signal {i}: len = {len(x[i])}')
    
    matrix = matrix_compare(y, dist_func)
    names = [file+'_'+str(n) for file, n in zip(sht_files, signals_n)]
    matrix = pd.DataFrame(matrix, index=names, columns=names)
    return matrix

current_dir = os.path.dirname(os.path.abspath(__file__))
os.chdir(current_dir)

sht_files = ['data/sht'+str(n)+'.sht' for n in [38916, 38917, 38918, 38919]]
signals_n = [20] * len(sht_files)

column_compare_matr = files_dtw(sht_files, signals_n, dtw_dist)
sns.heatmap(column_compare_matr, annot=True, fmt='.3f')
plt.show()
column_compare_matr.to_csv('output/column_compare.csv')




signals_n = [18, 19, 20, 26]
sht_files = ['data/sht'+str(n)+'.sht' for n in [38917]*len(signals_n)]

row_compare_matr = files_dtw(sht_files, signals_n, dtw_dist)
sns.heatmap(row_compare_matr, annot=True, fmt='.0f')
plt.show()
row_compare_matr.to_csv('output/row_compare.csv')

