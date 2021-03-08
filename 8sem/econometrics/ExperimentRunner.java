package com.company;

import org.apache.commons.math3.stat.inference.MannWhitneyUTest;
import org.ejml.simple.SimpleMatrix;

public class ExperimentRunner {
    private SimpleMatrix t = null;
    private SimpleMatrix g = null;

    ExperimentRunner(SimpleMatrix t, SimpleMatrix g) {
        this.t = t;
        this.g = g;
    }

    public void run(int maxDegree, int numIntervals) {
        int intervalSize = t.numRows() / numIntervals;
        for (int degree = 1; degree <= maxDegree; degree++) {
            System.out.println("Degree: " + degree);
            PolynomialApproximation p = new PolynomialApproximation(degree);

            for (int i = 0; i < numIntervals; i++) {
                int upperBound = i * intervalSize;
                int lowerBound = (i + 1) * intervalSize;
                SimpleMatrix tPart = t.extractMatrix(upperBound, lowerBound, 0, 1);
                SimpleMatrix gPart = g.extractMatrix(upperBound, lowerBound, 0, 1);

                SimpleMatrix betas = p.estimateBetasFrom(tPart, gPart);
                System.out.println("Interval: " + i);
                System.out.println("Estimated betas:");
                betas.print();

                estimateUnambiguity(tPart, gPart, p);
                System.out.println();
            }
        }
    }

    private void estimateUnambiguity(SimpleMatrix t, SimpleMatrix g, PolynomialApproximation p) {
        SimpleMatrix predicted = p.getValues(t);
        SimpleMatrix residuals = g.minus(predicted).elementPower(2);

        int halfSize = residuals.numRows() / 2;
        double [] firstHalf = Main.vectorToDoubleArray(
                residuals.extractMatrix(0, halfSize, 0, 1));
        double [] secondHalf = Main.vectorToDoubleArray(
                residuals.extractMatrix(halfSize, residuals.numRows(), 0, 1));
        MannWhitneyUTest test = new MannWhitneyUTest();
        double significanceLevel = test.mannWhitneyUTest(firstHalf, secondHalf);
        System.out.println("Statistic: " + test.mannWhitneyU(firstHalf, secondHalf));
        System.out.println("Significance level: " + significanceLevel);
        
        double mse = residuals.elementSum() / residuals.numRows();
        System.out.println("MSE: " + mse);
    }
}
