package com.company;

import org.ejml.simple.SimpleMatrix;

import java.io.File;
import java.io.FileNotFoundException;
import java.util.*;

public class Main {
    static final int MAX_DEGREE = 3;
    static final int NUM_INTERVALS_TO_CHECK = 5;

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
//        runGeneratedExperiments();
        runExperimentFromFile("file.txt");
    }

    public static void runGeneratedExperiments() {
        double left = 0, right = 30, step = 0.5;
        double [] coefs = new double[]{3, 0, 2, 25};

        for (double std = 0.01; std <= 100; std *= 100) {
            CumstomPolynomial cf = new CumstomPolynomial(std, coefs);

            List<Double> gs = new ArrayList<>();
            List<Double> ts = new ArrayList<>();
            for (double t = left; t < right; t += step) {
                ts.add(t);
                gs.add(cf.get(t));
            }

            SimpleMatrix tVector = listToVector(ts);
            SimpleMatrix gVector = listToVector(gs);
            System.out.println("Std: " + std);
            new ExperimentRunner(tVector, gVector).run(MAX_DEGREE, NUM_INTERVALS_TO_CHECK);
        }
    }

    // File must contain on 1st line t values separated by space
    // one 2nd line X(t) values separated by space
    public static void runExperimentFromFile(String filename) throws FileNotFoundException {
        Scanner sc = new Scanner(new File(filename));

        List<Double> t = new ArrayList<>();
        List<Double> X = new ArrayList<>();

	    if (sc.hasNextLine()) {
            String line = sc.nextLine();
            for (String el: line.split(" ")) {
                t.add(Double.parseDouble(el));
            }
        }

        if (sc.hasNextLine()) {
            String line = sc.nextLine();
            for (String el: line.split(" ")) {
                X.add(Double.parseDouble(el));
            }
        }

        new ExperimentRunner(listToVector(t), listToVector(X)).run(MAX_DEGREE, NUM_INTERVALS_TO_CHECK);
    }
}
