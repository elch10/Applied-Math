package com.company;

import java.util.Random;

public class CumstomPolynomial {
    private final Random r = new Random();
    private final double std;
    private final double[] coefs;

    public CumstomPolynomial(double std, double... coefs) {
        this.std = std;
        this.coefs = coefs.clone();
    }

    public double get(double t) {
        double res = 0.;
        for (int i = 0; i < this.coefs.length; i++) {
            res += this.coefs[i] * Math.pow(t, i);
        }
        res += this.r.nextGaussian() * this.std;
        return res;
    }
}