import numpy as np


n_vec = [20, 100]


def student_interval(x, n, t_quant=0.0):
    k = n - 1
    mean = np.sum(x) / n
    stand_dev = np.sqrt(np.sum((x - mean) ** 2) / n)

    left = mean - (stand_dev * t_quant) / np.sqrt(k)
    right = mean + (stand_dev * t_quant) / np.sqrt(k)

    left, right = np.around([left, right], decimals=4)

    return left, right


def chi_interval(x, n, ch_quant):
    k = n - 1
    mean = np.sum(x) / n
    stand_dev = np.sqrt(np.sum((x - mean) ** 2) / n)
    left = np.sqrt(k) * stand_dev / np.sqrt(ch_quant["0.975"])
    right = np.sqrt(k) * stand_dev / np.sqrt(ch_quant["0.025"])

    left, right = np.around([left, right], decimals=4)

    return left, right


def asympt_mean(x, n, norm_quant):
    mean = np.sum(x) / n
    stand_dev = np.sqrt(np.sum((x - mean) ** 2) / n)

    left = mean - stand_dev * norm_quant / np.sqrt(n)
    right = mean + stand_dev * norm_quant / np.sqrt(n)

    left, right = np.around([left, right], decimals=4)

    return left, right


def asympt_stand_dev(x, n, norm_quant):
    mean = np.sum(x) / n
    stand_dev = np.sqrt(np.sum((x - mean) ** 2) / n)
    moment_4 = np.sum((x - mean) ** 4) / n
    excess = moment_4 / (stand_dev ** 4) - 3

    left = stand_dev * (1 - 0.5 * norm_quant * np.sqrt((excess + 2) / n))
    right = stand_dev * (1 + 0.5 * norm_quant * np.sqrt((excess + 2) / n))

    left, right = np.around([left, right], decimals=4)

    return left, right


def main():
    stud_quant = {"19": 1.72, "99": 1.66}
    chi_quant = {
        "19": {
            "0.025": 8.91,
            "0.975": 32.85},
        "99": {
            "0.025": 73.12,
            "0.975": 128.4}
    }
    normal_quant = 1.96

    mu0 = 0
    sigma0 = 1

    print("Trusted intervals")
    for n in n_vec:
        x = np.random.normal(loc=mu0, scale=sigma0, size=n)
        print("mu, Student, n = {}:\n".format(n), student_interval(x, n, t_quant=stud_quant[str(n - 1)]))
        print("sigma, xhi^2, n = {}:\n".format(n), chi_interval(x, n, ch_quant=chi_quant[str(n - 1)]))

        print("Asymptotic estimations:")
        print("mu, n = {}:\n".format(n), asympt_mean(x, n, norm_quant=normal_quant))
        print("sigma, n = {}:\n".format(n), asympt_stand_dev(x, n, norm_quant=normal_quant))
        print()


if __name__ == "__main__":
    main()

