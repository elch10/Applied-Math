package ru.spbstu.pipeline;

import org.jetbrains.annotations.NotNull;
import ru.spbstu.pipeline.logging.Logger;

import java.io.*;
import java.util.ArrayList;
import java.util.List;

public class TrivialReader extends Loggable implements Reader {
    public enum Properties {
        INPUT_FILE,
        CHUNK_SIZE
    }

    ArrayList<Consumer> consumers = new ArrayList<>();
    String filename;
    byte[] data;
    int chunkSize;

    public TrivialReader(String config, Logger logger) {
        super(logger);
        ConfigStorage cfg = new ConfigStorage(config, new String[]{
                Properties.INPUT_FILE.toString(),
                Properties.CHUNK_SIZE.toString()
        });
        this.filename = cfg.getPropertyValue(Properties.INPUT_FILE.toString());
        this.chunkSize = Integer.parseInt(cfg.getPropertyValue(Properties.CHUNK_SIZE.toString()));
    }

    @Override
    public void run() {
        try (FileInputStream fin = new FileInputStream(filename)) {
            data = new byte[chunkSize];
            while (fin.read(data) > 0) {
                for (Consumer c: consumers) {
                    if (c.status() != Status.OK) {
                        status = Status.EXECUTOR_ERROR;
                        return;
                    }
                    c.loadDataFrom(this);
                    c.run();
                }
            }
        } catch (IOException e) {
            status = Status.ERROR;
            logger.log(e.toString());
        }
    }

    @Override
    public @NotNull byte[] get() {
        return data;
    }

    @Override
    public void addConsumer(Consumer consumer) {
        consumers.add(consumer);
    }

    @Override
    public void addConsumers(List<Consumer> consumers) {
        this.consumers.addAll(consumers);
    }
}
