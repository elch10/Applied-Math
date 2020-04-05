import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.patches import Ellipse
import matplotlib.transforms as transforms
from scipy import stats

from itertools import product

sizes = [20, 60, 100]
rs = [0, 0.5, 0.9]
repeats = 1000

def confidence_ellipse(x, y, mean, cov, ax, n_std=3.0, facecolor='none', **kwargs):
    """
    Create a plot of the covariance confidence ellipse of *x* and *y*.

    Parameters
    ----------
    x, y : array-like, shape (n, )
        Input data.

    ax : matplotlib.axes.Axes
        The axes object to draw the ellipse into.

    n_std : float
        The number of standard deviations to determine the ellipse's radiuses.

    Returns
    -------
    matplotlib.patches.Ellipse

    Other parameters
    ----------------
    kwargs : `~matplotlib.patches.Patch` properties
    """
    if x.size != y.size:
        raise ValueError("x and y must be the same size")

    # Using a special case to obtain the eigenvalues of this
    # two-dimensionl dataset.
    ell_radius_x = np.sqrt(1 + cov[0, 1])
    ell_radius_y = np.sqrt(1 - cov[0, 1])
    ellipse = Ellipse((0, 0), width=ell_radius_x * 2, height=ell_radius_y * 2,
                      edgecolor='b', lw=1,
                      facecolor=facecolor, **kwargs)

    # Calculating the stdandard deviation of x,y from
    # the squareroot of the variance and multiplying
    # with the given number of standard deviations.
    scale_x = np.sqrt(cov[0, 0]) * n_std
    scale_y = np.sqrt(cov[1, 1]) * n_std
    mean_x = mean[0]
    mean_y = mean[1]

    transf = transforms.Affine2D() \
        .rotate_deg(45) \
        .scale(scale_x, scale_y) \
        .translate(mean_x, mean_y)

    ellipse.set_transform(transf + ax.transData)
    return ax.add_patch(ellipse)

def confidence_ellipse_1(x, y, ax, n_std=3.0, facecolor='none', **kwargs):
    """
    Create a plot of the covariance confidence ellipse of *x* and *y*.

    Parameters
    ----------
    x, y : array-like, shape (n, )
        Input data.

    ax : matplotlib.axes.Axes
        The axes object to draw the ellipse into.

    n_std : float
        The number of standard deviations to determine the ellipse's radiuses.

    Returns
    -------
    matplotlib.patches.Ellipse

    Other parameters
    ----------------
    kwargs : `~matplotlib.patches.Patch` properties
    """
    if x.size != y.size:
        raise ValueError("x and y must be the same size")

    cov = np.cov(x, y)
    pearson = cov[0, 1]/np.sqrt(cov[0, 0] * cov[1, 1])
    # Using a special case to obtain the eigenvalues of this
    # two-dimensionl dataset.
    ell_radius_x = np.sqrt(1 + pearson)
    ell_radius_y = np.sqrt(1 - pearson)
    ellipse = Ellipse((0, 0), width=ell_radius_x * 2, height=ell_radius_y * 2,
                      edgecolor='b', lw=1,
                      facecolor=facecolor, **kwargs)

    # Calculating the stdandard deviation of x from
    # the squareroot of the variance and multiplying
    # with the given number of standard deviations.
    scale_x = np.sqrt(cov[0, 0]) * n_std
    mean_x = np.mean(x)

    # calculating the stdandard deviation of y ...
    scale_y = np.sqrt(cov[1, 1]) * n_std
    mean_y = np.mean(y)

    transf = transforms.Affine2D() \
        .rotate_deg(45) \
        .scale(scale_x, scale_y) \
        .translate(mean_x, mean_y)

    ellipse.set_transform(transf + ax.transData)
    return ax.add_patch(ellipse)


def quadrant_coeff(array):
    x = array[:, 0] - np.median(array[:, 0])
    y = array[:, 1] - np.median(array[:, 1])
    
    n1_3 = ((x > 0) & (y > 0)) | ((x < 0) & (y < 0))
    n2_4 = ((x < 0) & (y > 0)) | ((x > 0) & (y < 0))

    return (n1_3.sum() - n2_4.sum()) / array.shape[0]

def square_mean(array):
    return np.mean(array.values ** 2)

j = 0
for r, size in product(rs, sizes):
    mean = [0, 0]
    cov = np.array([[1, r], [r, 1]])

    j += 1
    ax = plt.subplot(3, 3, j)
    plt.title(f'R={r} Size={size}')
    data = np.random.multivariate_normal(mean, cov, size=size)
    ax.scatter(data[:, 0], data[:, 1], s=3)
    confidence_ellipse(data[:, 0], data[:, 1], mean, cov, ax)
    plt.xlim((-3, 3))
    plt.ylim((-3, 3))

    result = np.zeros((repeats, 3))
    # statistcs
    for i in range(repeats):
        data = np.random.multivariate_normal(mean, cov, size=size)
        result[i, 0] = stats.pearsonr(data[:, 0], data[:, 1])[0]
        result[i, 1] = stats.spearmanr(data)[0]
        result[i, 2] = quadrant_coeff(data)

    print(f'R={r} Size={size}')
    result = pd.DataFrame(result, columns=['Pearson', 'Spearman', 'Quadrant coeff'])
    res = result.aggregate([np.mean, square_mean, np.var], axis=0)
    print(res)
    print()

plt.show()

r=0.9
j=0
for size in sizes:
    mean = [0, 0]
    cov1 = np.array([[1, r], [r, 1]])
    cov2 = np.array([[1, -r*10], [-r*10, 1]])

    j += 1
    plt.suptitle('Mixing')
    ax = plt.subplot(2, 2, j)
    plt.title(f'Size={size}')
    data1 = np.random.multivariate_normal(mean, cov1, size=size)
    data2 = np.random.multivariate_normal(mean, cov2, size=size)
    data = 0.9 * data1 + 0.1 * data2
    ax.scatter(data[:, 0], data[:, 1], s=3)
    confidence_ellipse_1(data[:, 0], data[:, 1], ax)
    plt.xlim((-3, 3))
    plt.ylim((-3, 3))

    result = np.zeros((repeats, 3))
    # statistcs
    for i in range(repeats):
        data1 = np.random.multivariate_normal(mean, cov1, size=size)
        data2 = np.random.multivariate_normal(mean, cov2, size=size)
        data = 0.9 * data1 + 0.1 * data2
        result[i, 0] = stats.pearsonr(data[:, 0], data[:, 1])[0]
        result[i, 1] = stats.spearmanr(data)[0]
        result[i, 2] = quadrant_coeff(data)

    print(f'R={r} Size={size}')
    result = pd.DataFrame(result, columns=['Pearson', 'Spearman', 'Quadrant coeff'])
    res = result.aggregate([np.mean, square_mean, np.var], axis=0)
    print(res)
    print()

plt.show()