package com.company;

import org.ejml.simple.SimpleMatrix;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;
import java.util.*;

public class Main {
    static final int MAX_DEGREE = 3;

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

    public static void main(String[] args) throws IOException {
//        fillPolyFile("file.txt");
//        runPolyExperiment(args);
        fillExpFile("file.txt");
//        runExpExperiment(args);
        runAnnealingExperiment(args);
    }

    public static void fillPolyFile(String filename) throws IOException {
        int N = (int) 1E7;
        double left = 0, right = 200, step = (right - left) / N;

        double std = 1;
        double [] coefs = new double[]{5, 3, 7, 68};
        CumstomPolynomial cf = new CumstomPolynomial(std, coefs);

        try (FileWriter w = new FileWriter(filename, false)){
            w.write(N + "\n");

            SimpleMatrix t = new SimpleMatrix(N, 1);
            int k = 0;
            for (double value = left; value <= right; value += step) {
                w.write(String.valueOf(value) + " ");
                t.set(k, 0, value);
                k++;
            }

            w.write("\n");
            for (int i = 0; i < N; i++)
                w.write(String.valueOf(cf.get(t.get(i, 0))) + " ");
        }
    }

    public static void runPolyExperiment(String[] args) throws FileNotFoundException {
        System.out.println("Reading input...");

        Scanner sc = new Scanner(new File("file.txt"));

        int n = sc.nextInt();
        SimpleMatrix t = new SimpleMatrix(n, 1);
        for (int i = 0; i < n; i++) {
            t.set(i, 0, Double.parseDouble(sc.next()));
        }

        SimpleMatrix X = t.copy();
        for (int i = 0; i < n; i++) {
            X.set(i, 0, Double.parseDouble(sc.next()));
        }

        String outFilename = "result.txt";
        System.out.println("Running experiment... Result will be in " + outFilename);
        new ExperimentRunner(t, X).runPolys(MAX_DEGREE,
                Integer.parseInt(args[0]),
                outFilename);
        System.out.println("Done!");
    }

    public static void fillExpFile(String filename) throws IOException {
        int N = (int) 1E2;
        double left = 0, right = 200, step = (right - left) / N;

        double std = 1;
        CustomExponential cf = new CustomExponential(10, -5, std);

        try (FileWriter w = new FileWriter(filename, false)) {
            w.write(N + "\n");

            SimpleMatrix t = new SimpleMatrix(N, 1);
            int k = 0;
            for (double value = left; value < right; value += step) {
                w.write(String.valueOf(value) + " ");
                t.set(k, 0, value);
                k++;
            }

            w.write("\n");
            for (int i = 0; i < N; i++) {
                double val = cf.get(t.get(i, 0));
                w.write(String.valueOf(val) + " ");
            }
        }
    }

    public static void runExpExperiment(String[] args) throws FileNotFoundException {
        System.out.println("Reading input...");

        Scanner sc = new Scanner(new File("file.txt"));
        int n = sc.nextInt();
        SimpleMatrix t = new SimpleMatrix(n, 1);
        for (int i = 0; i < n; i++) {
            t.set(i, 0, Double.parseDouble(sc.next()));
        }

        SimpleMatrix X = t.copy();
        for (int i = 0; i < n; i++) {
            X.set(i, 0, Double.parseDouble(sc.next()));
        }

        String outFilename = "result.txt";
        System.out.println("Running experiment... Result will be in " + outFilename);
        new ExperimentRunner(t, X).runExps(5,
                Integer.parseInt(args[0]),
                outFilename);
        System.out.println("Done!");
    }

    public static void runAnnealingExperiment(String[] args) throws FileNotFoundException {
        System.out.println("Reading input...");

        Scanner sc = new Scanner(new File("file.txt"));
        int n = sc.nextInt();
        SimpleMatrix t = new SimpleMatrix(n, 1);
        for (int i = 0; i < n; i++) {
            t.set(i, 0, Double.parseDouble(sc.next()));
        }

        SimpleMatrix X = t.copy();
        for (int i = 0; i < n; i++) {
            X.set(i, 0, Double.parseDouble(sc.next()));
        }

        String outFilename = "result.txt";
        System.out.println("Running experiment... Result will be in " + outFilename);

        double T0 = 100;
        int K = 1000;
        new ExperimentRunner(t, X).runAnnealing(
                T0,
                new double[]{10, -10},
                K,
                outFilename);
        System.out.println("Done!");
    }
}
