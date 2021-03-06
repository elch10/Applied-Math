package com.company;

import org.ejml.simple.SimpleMatrix;

public class PolynomialApproximation {
    private final int degree;
    private SimpleMatrix betas = null;

    public PolynomialApproximation(int degree) {
        this.degree = degree;
    }

    public SimpleMatrix estimateBetasFrom(SimpleMatrix t, SimpleMatrix g_t) {
        if (t.numRows() != g_t.numRows() || t.numCols() != 1 || g_t.numCols() != 1) {
            return null;
        }

        SimpleMatrix z = createZMatrix(t);
        SimpleMatrix z_zt = z.mult(z.transpose());
        SimpleMatrix b = z.mult(g_t);

        this.betas = z_zt.pseudoInverse().mult(b);
        return this.betas;
    }

    private SimpleMatrix createZMatrix(SimpleMatrix t) {
        double [][] z_data = new double[this.degree+1][t.numRows()];
        for (int i = 0; i < this.degree+1; i++) {
            for (int j = 0; j < t.numRows(); j++) {
                z_data[i][j] = Math.pow(t.get(j, 0), i);
            }
        }
        return new SimpleMatrix(z_data);
    }

    public SimpleMatrix getBetas() {
        return betas;
    }

    public SimpleMatrix getValues(SimpleMatrix t) {
        SimpleMatrix z = createZMatrix(t);
        return z.transpose().mult(this.betas);
    }

    public int getDegree() {
        return degree;
    }
}