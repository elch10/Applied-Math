package com.company;

import java.io.*;
import java.lang.reflect.Method;
import java.util.logging.*;

public class Main {
    private static Logger log;

    private static void tieLogger(String filename) {
        log = Logger.getLogger(Main.class.getName());
        log.setUseParentHandlers(false);

        try {
            FileHandler fh = new FileHandler(filename);
            log.addHandler(fh);

            SimpleFormatter formatter = new SimpleFormatter();
            fh.setFormatter(formatter);
        } catch (IOException e) {
            System.err.println("Logger cannot be initialized. Continue without logging...");
            System.err.println(e.getMessage());
            log = null;
        }
    }

    static void writeToLog(Level level, String message) {
        if (log != null) {
            log.log(level, message);
        }
    }

    private static boolean run(ConfigStorage cfg) {
        RunLengthEncoding rle = new RunLengthEncoding();

        try (FileReader originalContent = new FileReader(cfg.getPropertyValue(
                ConfigStorage.ConfigProperty.INPUT_FILE.toString()));
             BufferedReader in = new BufferedReader(originalContent);
             FileWriter fOut = new FileWriter(cfg.getPropertyValue(
                     ConfigStorage.ConfigProperty.OUTPUT_FILE.toString()))) {

            String currentMode = cfg.getPropertyValue(ConfigStorage.ConfigProperty.MODE.toString());

            if (currentMode.equals(RunLengthEncoding.Mode.Encode.toString())) {
                for (String line = in.readLine(); line != null; line = in.readLine()) {
                    fOut.write(rle.encode(line));
                    fOut.write('\n');
                }
            } else if (currentMode.equals(RunLengthEncoding.Mode.Decode.toString())){
                for (String line = in.readLine(); line != null; line = in.readLine()) {
                    fOut.write(rle.decode(line));
                    fOut.write('\n');
                }
            } else {
                writeToLog(Level.SEVERE, "Mode - \"" + currentMode + "\" doesn't exist");
                return false;
            }
        } catch (IOException e) {
            writeToLog(Level.SEVERE, "Something went wrong: " + e.getMessage());
            return false;
        }
        return true;
    }

// You can use this shit for testing purposes
//    private static boolean checkEncoding(ConfigStorage cfg) throws IOException {
//        RunLengthEncoding rle = new RunLengthEncoding();
//
//        String tempFileName = "temp.txt";
//
//        // Decode to temp file and compare encoded and then decoded from original content
//        try (FileReader encoded = new FileReader(cfg.getPropertyValue(
//                ConfigStorage.ConfigProperty.OUTPUT_FILE.getName()));
//             BufferedReader in = new BufferedReader(encoded);
//             FileWriter tempOut = new FileWriter(tempFileName)) {
//
//            for (String line = in.readLine(); line != null; line = in.readLine()) {
//                tempOut.write(rle.decode(line));
//            }
//
//            tempOut.close();
//
//            try (FileInputStream tempIn = new FileInputStream(tempFileName);
//                 FileInputStream originalContent = new FileInputStream(cfg.getPropertyValue(
//                    ConfigStorage.ConfigProperty.INPUT_FILE.getName()))) {
//
//                int data;
//                while ((data = originalContent.read()) != -1) {
//                    if (data != tempIn.read()) {
//                        return false;
//                    }
//                }
//
//            }
//        } finally {
//            File f = new File(tempFileName);
//            f.delete();
//        }
//
//        return true;
//    }

    public static void main(String[] args) {
        if (args.length != 1) {
            System.err.println("Current app accepts only 1 argument - path to the config file");
            return;
        }

        ConfigStorage cfg = new ConfigStorage(args[0]);

        if (!cfg.isStatusOk()) {
            System.err.println("Config file was not read properly. Check the log file");
            return;
        }

        tieLogger(cfg.getPropertyValue(ConfigStorage.ConfigProperty.LOG_FILE.toString()));

        if (!run(cfg)) {
            System.err.println("During Encoding/Decoding error occurred. See the log file");
            return;
        }
//        if (!checkEncoding(cfg)) {
//            System.err.println("Encoded then decoded file is not equal to INPUT_FILE");
//            return;
//        }

        System.out.println("Done");
    }

    private static String generateStackTrace(Exception exception) {
        StringWriter sw = new StringWriter();
        exception.printStackTrace(new PrintWriter(sw));
        return sw.toString();
    }
}
