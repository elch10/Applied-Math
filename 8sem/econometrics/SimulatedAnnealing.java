package com.company;

import org.apache.commons.math3.distribution.MultivariateNormalDistribution;
import org.ejml.simple.SimpleMatrix;

import java.util.Random;

public class SimulatedAnnealing {
    double T0, Emin;
    SimpleMatrix t, X;
    double[] x, xMin;
    int k = 1;
    Random r = new Random();

    public SimulatedAnnealing(double T0, double[] x0,
                              SimpleMatrix t, SimpleMatrix X) {
        this.T0 = T0;
        this.x = x0;
        this.t = t;
        this.X = X;
        Emin = getSSE(x0);
        xMin = x0;
    }

    public double[] sampleNextPoint() {
        double [][] cov = new double[x.length][x.length];
        double currentTemperature = getT();
        for (int i = 0; i < x.length; i++) {
            cov[i][i] = currentTemperature;
        }

        MultivariateNormalDistribution mnv =
                new MultivariateNormalDistribution(x, cov);
        return mnv.sample();
    }

    public double getH(double SSEDelta) {
        return Math.exp(-SSEDelta / getT());
    }

    public double[] getLastPoint() {
        return x;
    }

    public double[] getGlobalXMin() {
        return xMin;
    }

    void makeIteration() {
        int steps = 0;
        double E = getSSE(x);
        while (true) {
            double[] xCandidate = sampleNextPoint();
            double ECandidate = getSSE(xCandidate),
                    alpha = r.nextDouble();
            double SSEDelta = ECandidate - E;
            if (alpha < getH(SSEDelta)) {
                x = xCandidate;
                break;
            }
            steps++;
        }
        E = getSSE(x);
        if (E < Emin) {
            Emin = E;
            xMin = x;
        }
        makeStepK();
    }

    private void makeStepK() {
        k++;
    }

    public double getT() {
        return T0 / Math.log(1 + k);
    }

    public double getSSE(double[] beta) {
        CustomExponential ce = CustomExponential.fromDoubleArray(beta);
        return X.minus(ce.getValues(t)).elementPower(2).elementSum();
    }
}
