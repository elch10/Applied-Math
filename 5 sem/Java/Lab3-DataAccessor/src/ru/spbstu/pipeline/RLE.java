package ru.spbstu.pipeline;

import org.jetbrains.annotations.NotNull;
import ru.spbstu.pipeline.logging.Logger;

import java.io.ByteArrayOutputStream;
import java.io.UnsupportedEncodingException;
import java.lang.reflect.Constructor;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Set;

public class RLE extends Loggable implements Executor {
    public enum Modes {
        ENCODE, DECODE
    }

    public enum Properties {
        MODE,
        CHUNK_SIZE
    }

    static final private HashMap<String, Class> DATA_TYPES = new HashMap<String, Class>() {{
        put(byte[].class.getCanonicalName(), ByteAccessor.class);
        put(char[].class.getCanonicalName(), CharAccessor.class);
        put(String.class.getCanonicalName(), StringAccessor.class);
    }};

    private ArrayList<Consumer> consumers;
    private DataAccessor producerAccessor;
    private byte[] data, processed;
    private String mode;
    private int chunkSize;

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
    public long loadDataFrom(Producer producer) {
        data = (byte[])producerAccessor.get();
        return producerAccessor.size();
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
            long loaded = c.loadDataFrom(this);
            if (c.status() != Status.OK || loaded == 0) {
                status = c.status();
                return;
            }
            c.run();
            status = c.status();
        }
    }

    @Override
    public void addProducer(Producer producer) {
        if (!producer.outputDataTypes().contains(byte[].class.getCanonicalName())) {
            status = Status.EXECUTOR_ERROR;
            return;
        }
        producerAccessor = producer.getAccessor(byte[].class.getCanonicalName());
    }

    @Override
    public void addProducers(List<Producer> producers) {}

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

    @Override
    public @NotNull Set<String> outputDataTypes() {
        return DATA_TYPES.keySet();
    }

    @Override
    public @NotNull DataAccessor getAccessor(@NotNull String s) {
        try {
            Constructor c = DATA_TYPES.get(s).getDeclaredConstructor(this.getClass());
            c.setAccessible(true);
            return (DataAccessor) c.newInstance(this);
        } catch (Exception e) {
            logger.log(e.toString());
            status = Status.EXECUTOR_ERROR;
        }
        return null;
    }

    private class ByteAccessor implements DataAccessor {
        @Override
        public @NotNull Object get() {
            byte[] copy = new byte[processed.length];
            System.arraycopy(processed, 0, copy, 0, processed.length);
            return copy;
        }

        @Override
        public long size() {
            return chunkSize;
        }
    }

    private class CharAccessor implements DataAccessor {
        @Override
        public @NotNull Object get() {
            try {
                return new String(processed, "UTF-8").toCharArray();
            } catch (UnsupportedEncodingException e) {
                status = Status.EXECUTOR_ERROR;
                logger.log(e.toString());
            }
            return new char[0];
        }

        @Override
        public long size() {
            return chunkSize;
        }
    }

    private class StringAccessor implements DataAccessor {
        @Override
        public @NotNull Object get() {
            try {
                return new String(processed, "UTF-8");
            } catch (UnsupportedEncodingException e) {
                status = Status.EXECUTOR_ERROR;
                logger.log(e.toString());
            }
            return "";
        }

        @Override
        public long size() {
            return chunkSize;
        }
    }
}
