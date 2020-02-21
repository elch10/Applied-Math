package ru.spbstu.pipeline;

import ru.spbstu.pipeline.logging.Logger;

import java.util.ArrayList;

public class Manager extends ConfigStorage implements Runnable {
    public static final String ARGUMENTS_DELIMITER = " ";
    public static final String READER = "reader";
    public static final String WRITER = "writer";

    private ArrayList<Executor> executors;
    private TrivialReader reader;
    private TrivialWriter writer;

    public Manager(String configFileName) {
        super(configFileName, new String[]{});
    }

    @Override
    public boolean processLine(String line) {
        if (executors == null) {
            executors = new ArrayList<>();
        }

        String[] args = line.split(ARGUMENTS_DELIMITER);
        if (args.length != 2) {
            setBadStatus("You need to specify path to executor and path to config");
            return false;
        }

        String lower = args[0].toLowerCase();

        if (lower.contains(READER)) {
            reader = new TrivialReader(args[1], Main.logger);
        } else if (lower.contains(WRITER)) {
            writer = new TrivialWriter(args[1], Main.logger);
        } else {
            try {
                Class cls = Class.forName(args[0]);
                Object obj = cls.getConstructor(String.class, Logger.class).newInstance(args[1], Main.logger);
                executors.add((Executor)obj);
            } catch (Exception e) {
                setBadStatus(e.getMessage());
                return false;
            }
        }


        return true;
    }

    @Override
    public void run() {
        if (executors.isEmpty()) {
            reader.addConsumer(writer);
        } else {
            reader.addConsumer(executors.get(0));
            for (int i = 0; i < executors.size() - 1; ++i) {
                executors.get(i).addConsumer(executors.get(i+1));
            }
            executors.get(executors.size()-1).addConsumer(writer);
        }
        reader.run();
        writer.close();
    }
}
