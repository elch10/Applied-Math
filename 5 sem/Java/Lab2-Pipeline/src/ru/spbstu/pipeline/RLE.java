package ru.spbstu.pipeline;

import org.jetbrains.annotations.NotNull;
import ru.spbstu.pipeline.logging.Logger;

import java.io.ByteArrayOutputStream;
import java.util.ArrayList;
import java.util.List;

public class RLE extends Loggable implements Executor {
    public enum Modes {
        ENCODE, DECODE
    };

    public enum Properties {
        MODE,
        CHUNK_SIZE
    };

    ArrayList<Consumer> consumers;
    byte[] data, processed;
    String mode;
    int chunkSize;


    /* I suppose that every executor can take several arguments in the
     * `manager` config file that is read by Manager
     */
    public RLE(String conf, Logger logger) {
        super(logger);
        consumers = new ArrayList<>();
        ConfigStorage cfg = new ConfigStorage(conf, new String[]{
                Properties.MODE.toString(),
                Properties.CHUNK_SIZE.toString(),
        });
        mode = cfg.getPropertyValue(Properties.MODE.toString());
        chunkSize = Integer.parseInt(cfg.getPropertyValue(Properties.CHUNK_SIZE.toString()));
    }


    void encode() {
        ByteArrayOutputStream b = new ByteArrayOutputStream();
        byte code = 0, count = 0;
        for (int i = 0; i < data.length; ++i) {
            if (i == 0 || data[i-1] != data[i] || count == 255) {
                if (i != 0) {
                    b.write(code);
                    b.write(count);
                }
                code = data[i];
                count = 1;
                continue;
            }
            count++;
        }
        b.write(code);
        b.write(count);
        processed = b.toByteArray();
    }

    void decode() {
        ByteArrayOutputStream b = new ByteArrayOutputStream();
        for (int i = 0; i < data.length; i += 2) {
            byte code = data[i], count = data[i+1];
            for (int j = 0; j < count; ++j) {
                b.write(code);
            }
        }
        processed = b.toByteArray();
    }

    @Override
    public void loadDataFrom(Producer producer) {
        data = producer.get();
    }

    @Override
    public void run() {
        if (mode.equals(Modes.ENCODE.toString())) {
            encode();
        } else if (mode.equals(Modes.DECODE.toString())) {
            decode();
        } else {
            status = Status.EXECUTOR_ERROR;
            return;
        }
        for (Consumer c: consumers) {
            if (c.status() != Status.OK) {
                status = Status.ERROR;
                return;
            }
            c.loadDataFrom(this);
            c.run();
        }
    }

    @Override
    public void addProducer(Producer producer) {}

    @Override
    public void addProducers(List<Producer> producers) {}

    @Override
    public @NotNull byte[] get() {
        return processed;
    }

    @Override
    public void addConsumer(Consumer consumer) {
        consumers.add(consumer);
    }

    @Override
    public void addConsumers(List<Consumer> consumers) {
        this.consumers.addAll(consumers);
    }

    @Override
    public Status status() {
        return status;
    }
}
