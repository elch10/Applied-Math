package ru.spbstu.pipeline;

import org.jetbrains.annotations.NotNull;
import ru.spbstu.pipeline.logging.Logger;

import java.io.*;
import java.lang.reflect.Constructor;
import java.lang.reflect.Method;
import java.util.*;

public class TrivialReader extends Loggable implements Reader {
    public enum Properties {
        INPUT_FILE,
        CHUNK_SIZE
    }

    static final private HashMap<String, Class> DATA_TYPES = new HashMap<String, Class>() {{
        put(byte[].class.getCanonicalName(), ByteAccessor.class);
        put(char[].class.getCanonicalName(), CharAccessor.class);
        put(String.class.getCanonicalName(), StringAccessor.class);
    }};

    private ArrayList<Consumer> consumers = new ArrayList<>();
    private String filename;
    private byte[] data;
    private int chunkSize;

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
                    long loaded = c.loadDataFrom(this);
                    if (c.status() != Status.OK || loaded == 0) {
                        status = c.status();
                        return;
                    }
                    c.run();
                    status = c.status();
                }
            }
        } catch (IOException e) {
            status = Status.ERROR;
            logger.log(e.toString());
        }
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
    public @NotNull DataAccessor getAccessor(@NotNull String s) {
        try {
            Constructor c = DATA_TYPES.get(s).getDeclaredConstructor(this.getClass());
            c.setAccessible(true);
            return (DataAccessor) c.newInstance(this);
        } catch (Exception e) {
            logger.log(e.toString());
            status = Status.READER_ERROR;
        }
        return null;
    }

    @Override
    public @NotNull Set<String> outputDataTypes() {
        return DATA_TYPES.keySet();
    }

    private class ByteAccessor implements DataAccessor {
        @Override
        public @NotNull Object get() {
            return data.clone();
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
                return new String(data, "UTF-8").toCharArray();
            } catch (UnsupportedEncodingException e) {
                status = Status.READER_ERROR;
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
                return new String(data, "UTF-8");
            } catch (UnsupportedEncodingException e) {
                status = Status.READER_ERROR;
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
