import math
import numpy as np
import scipy.stats as st
import matplotlib.pyplot as plt
import seaborn as sns

sns.set_style("darkgrid")
n = 100
k = int(1 + 3.3 * math.log(n))  # = 16

# quantile for k = 16 and alpha = 0.05:
# 26,296


def maximal_likelihood(x):
    mu0 = 0
    sigma0 = 1

    x_plot = np.linspace(-2, 2, n)

    mu_hat = np.mean(x)
    sigma_hat = np.mean((x - np.full(n, mu_hat)) ** 2)
    plt.plot(x_plot, st.norm.pdf(x_plot, mu0, sigma0),
             label='Распределение гаусса', color='olivedrab')
    plt.plot(x_plot, st.norm.pdf(x_plot, mu_hat, sigma_hat),
             label='Оценка максимального правдаподобия', color='indianred')
    plt.legend()
    plt.show()
    
    return mu_hat, sigma_hat


def chi_sqr_criterion(x, mu_hat, sigma_hat):
    print("k = ", k)

    chunks = np.linspace(-3.5, 3.5, k - 2)
    chunks = np.insert(chunks, 0, -np.inf)
    chunks = np.append(chunks, np.inf)

    print("Intervals for chi square criterion:")

    p_chunks = np.ndarray(shape=(k - 1, 1))
    for i in range(1, k):
        p_chunks[i - 1] = st.norm.cdf(chunks[i], loc=mu_hat, scale=sigma_hat) \
                          - st.norm.cdf(chunks[i - 1], loc=mu_hat, scale=sigma_hat)
        print("Interval: %.2f %.2f" % (chunks[i - 1], chunks[i]))
        print("%.5f" % p_chunks[i - 1][0], "\n")

    print("Sum p: ", np.sum(p_chunks))
    freq_i = np.zeros(shape=(k - 1, 1))
    for i in range(k - 1):
        for j in range(0, n):
            if chunks[i] < x[j] and x[j] < chunks[i + 1]:
                freq_i[i] += 1

    chi_sq = 0
    for i in range(k - 1):
        chi_sq += ((freq_i[i] - n * p_chunks[i]) ** 2) / (n * p_chunks[i])

    print("k      Interval         n_i     p_i         n_i - np_i  (n_i - np_i)^2 /(np_i) ")
    for i in range(k - 1):
        print("%2s & " % str(i + 1),
              "%5s ; " % str("%.2f" % chunks[i]),
              "%5s & " % str("%.2f" % chunks[i + 1]),
              "%3s & " % str("%.0f" % freq_i[i]), 
              "%.5f & " % p_chunks[i],
              "%7s & " % str("%.2f" % (freq_i[i] - n * p_chunks[i])),
              "%.2f \\\\ \hline" % (((freq_i[i] - n * p_chunks[i]) ** 2) / (n * p_chunks[i])))

    print("sum &   –––––––––––   & ",
          "%.0f & " % np.sum(freq_i),
          "%7s & " % str("%.0f" % np.sum(p_chunks)),
          "%7s & " % str("%.2f" % np.sum(freq_i - n * p_chunks)),
          "%.2f" % np.sum(((freq_i - n * p_chunks) ** 2) / (n * p_chunks)))


def main():
    mu0, sigma0 = 0, 1

    x = np.random.normal(loc=mu0, scale=sigma0, size=n)
    mu_hat, sigma_hat = maximal_likelihood(x)
    chi_sqr_criterion(x, mu_hat, sigma_hat)


if __name__ == "__main__":
    main()

