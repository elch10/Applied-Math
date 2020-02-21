package ru.spbstu.pipeline;

import ru.spbstu.pipeline.logging.UtilLogger;

import java.io.*;
import java.util.logging.*;

public class Main {

    public static UtilLogger logger;
    public static final String LOG_FILENAME = "log.log";

    static {
        Logger log = Logger.getLogger(Main.class.getName());
        log.setUseParentHandlers(false);

        try {
            FileHandler fh = new FileHandler(LOG_FILENAME);
            log.addHandler(fh);

            SimpleFormatter formatter = new SimpleFormatter();
            fh.setFormatter(formatter);
        } catch (IOException e) {
            System.err.println("Logger cannot be initialized. Continue without logging...");
            System.err.println(e.getMessage());
            log = null;
        }
        logger = UtilLogger.of(log);
    }

    public static void writeToLog(String message) {
        logger.log(message);
    }
    public static void main(String[] args) {

        if (args.length != 1) {
            System.err.println("Current app accepts only 1 argument - path to the config file");
            return;
        }

        Manager manager = new Manager(args[0]);

        if (!manager.isStatusOk()) {
            System.err.println("Config file was not read properly. Check the log file");
            return;
        }

        manager.run();

        System.out.println("Done");
    }


}

