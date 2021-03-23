package com.company;

import org.ejml.simple.SimpleMatrix;

import java.util.Random;

public class CustomExponential {
    private final Random r = new Random();
    private double std;
    double beta0, beta1;

    public CustomExponential(double beta0, double beta1, double std) {
        this.beta0 = beta0;
        this.beta1 = beta1;
        this.std = std;
    }

    public double get(double t) {
        double res = this.beta0 * Math.exp(this.beta1 * t);
        res += r.nextGaussian() * std;
        return res;
    }

    public SimpleMatrix getValues(SimpleMatrix t) {
        SimpleMatrix values = t.copy();
        for (int i = 0; i < t.numRows(); i++) {
            values.set(i, 0, get(t.get(i, 0)));
        }
        return values;
    }

    void setBetas(SimpleMatrix s) {
        beta0 = s.get(0, 0);
        beta1 = s.get(0, 1);
    }

    public static CustomExponential fromDoubleArray(double [] betas) {
        return new CustomExponential(betas[0], betas[1], 0);
    }
}
