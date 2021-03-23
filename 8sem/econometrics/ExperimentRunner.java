package com.company;

import org.apache.commons.math3.stat.inference.MannWhitneyUTest;
import org.ejml.simple.SimpleMatrix;

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;

public class ExperimentRunner {

    public static final double SIGNIFICANCE_LEVEL = 0.05;

    private final SimpleMatrix t;
    private final SimpleMatrix X;

    ExperimentRunner(SimpleMatrix t, SimpleMatrix X) {
        this.t = t;
        this.X = X;
    }

    public void runPolys(int maxDegree, int intervalSize, String logFilename) {
        try (BufferedWriter bf = new BufferedWriter(new FileWriter(logFilename))) {
            for (int degree = 0; degree <= maxDegree; degree++) {
                PolynomialApproximation p = new PolynomialApproximation(degree);

                int ambiguousNum = 0;
                for (int i = 0; i < t.numRows(); i += intervalSize) {
                    int upperBound = i;
                    int lowerBound = i + intervalSize;
                    SimpleMatrix tPart = t.extractMatrix(upperBound, lowerBound, 0, 1);
                    SimpleMatrix XPart = X.extractMatrix(upperBound, lowerBound, 0, 1);

                    SimpleMatrix betas = p.estimateBetasFrom(tPart, XPart);
                    SimpleMatrix residuals = XPart.minus(p.getValues(tPart));
                    double pValue = estimateUnambiguity(tPart, XPart, p);
                    double sigma = estimateSigma(tPart, XPart, p);

                    if (pValue < SIGNIFICANCE_LEVEL) {
                        ambiguousNum++;
                    }

                    bf.write("p1 Degree: " + degree + "\n");
                    bf.write("p1 Interval: " + i / intervalSize + "\n");
                    bf.write("p1 Betas: ");
                    for (int j = 0; j < betas.numRows(); j++) {
                        bf.write(betas.get(j, 0) + " ");
                    }
                    bf.write("\n");
                    bf.write("p1 Estimated sigma: " + sigma + "\n");

                    bf.write("p2 Residuals: ");
                    for (int j = 0; j < residuals.numRows(); j++) {
                        bf.write(residuals.get(j, 0) + " ");
                    }
                    bf.write("\n");
                    bf.write("p2 P-value: " + pValue + "\n\n");
                }
                double numIntervals = (double)t.numRows() / intervalSize;
                double share = ((double)ambiguousNum / numIntervals);
                bf.write("Share of accepting alternative hypothesis " +
                        "(unambiguous residuals): " + share);
                bf.write("\n\n\n");
                bf.flush();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private double estimateUnambiguity(SimpleMatrix t, SimpleMatrix X, PolynomialApproximation p) {
        SimpleMatrix residuals = X.minus(p.getValues(t));
        int halfSize = residuals.numRows() / 2;
        double [] firstHalf = Main.vectorToDoubleArray(
                residuals.extractMatrix(0, halfSize, 0, 1));
        double [] secondHalf = Main.vectorToDoubleArray(
                residuals.extractMatrix(halfSize, residuals.numRows(), 0, 1));

        MannWhitneyUTest test = new MannWhitneyUTest();
        return test.mannWhitneyUTest(firstHalf, secondHalf);
    }

    private double estimateSigma(SimpleMatrix t, SimpleMatrix X, PolynomialApproximation p) {
        double sum = X.minus(p.getValues(t)).elementPower(2).elementSum();
        return sum / (t.numRows() - p.getDegree() - 1);
    }

    public void runExps(int degree, int intervalSize, String logFilename) {
        try (BufferedWriter bf = new BufferedWriter(new FileWriter(logFilename))) {
            PolynomialApproximation p = new PolynomialApproximation(degree);

            for (int i = 0; i < t.numRows(); i += intervalSize) {
                int upperBound = i;
                int lowerBound = i + intervalSize;
                SimpleMatrix tPart = t.extractMatrix(upperBound, lowerBound, 0, 1);
                SimpleMatrix XPart = X.extractMatrix(upperBound, lowerBound, 0, 1);

                SimpleMatrix gammas = p.estimateBetasFrom(tPart, XPart);

                bf.write("p1 Degree: " + degree + "\n");
                bf.write("p1 Interval: " + i / intervalSize + "\n");
                bf.write("p1 Gammas: ");
                for (int j = 0; j < gammas.numRows(); j++) {
                    bf.write(gammas.get(j, 0) + " ");
                }
                bf.write("\n");
                bf.write("p1 Betas: ");
                for (int j = 0; j < gammas.numRows(); j++) {
                    double gamma = gammas.get(j, 0);
                    double beta = 0;
                    if (j == 0) {
                        beta = 1;
                    } else {
                        beta = Math.pow(factorial(j) * gamma, 1. / j);
                    }
                    bf.write(beta + " ");
                }
                bf.write("\n\n");
            }
            bf.write("\n\n\n");
            bf.flush();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private static double factorial(int n) {
        double fact = 1;
        for(int i = 1; i <= n; i++) {
            fact *= i;
        }
        return fact;
    }

    public void runAnnealing(double T0, double[] x0, int K, String logFilename) {
        try (BufferedWriter bf = new BufferedWriter(new FileWriter(logFilename))) {
            SimulatedAnnealing sa = new SimulatedAnnealing(T0, x0, t, X);
            for (int i = 0; i < K; i++) {
                sa.makeIteration();

                double[] lastPoint = sa.getLastPoint();
                bf.write("Step: " + i + "\n");
                bf.write("Last point:\n");
                for (int j = 0; j < lastPoint.length; j++) {
                    bf.write(lastPoint[j] + " ");
                }
                bf.write("\n");
                bf.write("SSE at last point: " + sa.getSSE(lastPoint) + "\n");

                double[] xMin = sa.getGlobalXMin();
                bf.write("Global min point:\n");
                for (int j = 0; j < xMin.length; j++) {
                    bf.write(xMin[j] + " ");
                }
                bf.write("\n");
                bf.write("SSE at global min point: " + sa.getSSE(xMin) + "\n");
                bf.write("\n");
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
