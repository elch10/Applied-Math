package ru.spbstu.pipeline;

import ru.spbstu.pipeline.logging.Logger;

import java.io.FileOutputStream;
import java.io.IOException;
import java.util.List;

public class TrivialWriter extends Loggable implements Writer {
    public enum Properties {
        OUTPUT_FILE,
        CHUNK_SIZE
    };

    private FileOutputStream fOut;;
    private byte[] data;
    private int chunkSize;
    private Producer.DataAccessor producerAccessor;


    public TrivialWriter(String conf, Logger logger) {
        super(logger);
        ConfigStorage cfg = new ConfigStorage(conf, new String[]{
                Properties.OUTPUT_FILE.toString(),
                Properties.CHUNK_SIZE.toString(),
        });
        this.chunkSize = Integer.parseInt(cfg.getPropertyValue(TrivialReader.Properties.CHUNK_SIZE.toString()));
        try {
            fOut = new FileOutputStream(cfg.getPropertyValue(Properties.OUTPUT_FILE.toString()));
        } catch (IOException e) {
            this.status = Status.WRITER_ERROR;
        }
    }

    @Override
    public long loadDataFrom(Producer producer) {
        data = (byte[]) producerAccessor.get();
        return producerAccessor.size();
    }

    public void close() {
        try {
            fOut.close();
        } catch (IOException e) {
            this.status = Status.WRITER_ERROR;
            logger.log(e.toString());
        }
    }

    @Override
    public void run() {
        try {
            fOut.write(data);
        } catch (IOException e) {
            this.status = Status.WRITER_ERROR;
            logger.log(e.toString());
        }
    }

    @Override
    public Status status() {
        return status;
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
}
