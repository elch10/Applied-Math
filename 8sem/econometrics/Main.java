package com.company;

import org.ejml.simple.SimpleMatrix;
import org.knowm.xchart.SwingWrapper;
import org.knowm.xchart.XYChart;

import java.io.FileNotFoundException;
import java.util.*;

public class Main {
    public static SimpleMatrix listToVector(List<Double> elems) {
        SimpleMatrix m = new SimpleMatrix(elems.size(), 1);
        for (int i = 0; i < elems.size(); i++) {
            m.set(i, 0, elems.get(i));
        }
        return m;
    }

    public static double[] vectorToDoubleArray(SimpleMatrix elems) {
        if (elems.numCols() != 1) {
            return null;
        }
        double [] ar = new double[elems.numRows()];
        for (int i = 0; i < elems.numRows(); i++) {
            ar[i] = elems.get(i, 0);
        }
        return ar;
    }

    public static void main(String[] args) throws FileNotFoundException {
        double left=0, right = 40, step = 0.2;

        double [] coefs = new double[]{5, 10};

        for (double std = 10; std <= 10; std *= 10) {
            CustomFunction cf = new CustomFunction(std, coefs);

            List<Double> gs = new ArrayList<>();
            List<Double> ts = new ArrayList<>();
            for (double t = left; t < right; t += step) {
                ts.add(t);
                gs.add(cf.get(t));
            }

            System.out.println("Real std:" + std);
            System.out.println("Real coefs:" + Arrays.toString(coefs));
            SimpleMatrix tVector = listToVector(ts);
            SimpleMatrix gtVector = listToVector(gs);
            fitPolys(tVector, gtVector, 3);
            System.out.println();
        }

    }

    private static void fitPolys(SimpleMatrix ts, SimpleMatrix gs, int maxDegree) {
        XYChart chart = new XYChart(800, 600);
//        chart.getStyler().setYAxisLogarithmic(true);
        chart.addSeries("Real",
                vectorToDoubleArray(ts), vectorToDoubleArray(gs));

        for (int degree = 1; degree <= maxDegree; degree++) {
            PolynomialApproximation p = new PolynomialApproximation(degree);
            SimpleMatrix betas = p.estimateBetasFrom(ts, gs);
            System.out.println("Degree: " + degree);
            System.out.println("Estimated betas:");
            betas.print();

            SimpleMatrix predicted = p.getValues(ts);
            double diff = predicted.minus(gs).elementPower(2).elementSum();
            double dispersion = diff / ts.numRows();
            System.out.println("Estimated dispersion: " + dispersion);
            System.out.println();


            chart.addSeries("Degree: "+degree,
                    vectorToDoubleArray(ts), vectorToDoubleArray(predicted));
        }

        new SwingWrapper(chart).displayChart();
    }
}
