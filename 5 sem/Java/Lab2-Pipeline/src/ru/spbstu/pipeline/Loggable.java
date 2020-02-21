package ru.spbstu.pipeline;

import ru.spbstu.pipeline.logging.Logger;

public class Loggable {
    protected Logger logger;
    protected Status status;
    public Loggable(Logger logger) {
        this.logger = logger;
        status = Status.OK;
    }
}
