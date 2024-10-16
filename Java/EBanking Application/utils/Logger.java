package org.poo.cb.utils;

import java.io.FileWriter;
import java.util.logging.FileHandler;
import java.util.logging.Handler;
import java.util.logging.Level;
import java.util.logging.SimpleFormatter;

public class Logger {
    private static java.util.logging.Logger logger = null;
    public static java.util.logging.Logger getInstace() {
        if (logger == null) {
            new Logger();
        }
        return logger;
    }
    private Logger() {
        logger = java.util.logging.Logger.getLogger("Bank");
        try {
            /* Clear log file */
            new FileWriter("log.txt", false).close();
            Handler fileHandler = new FileHandler("log.txt", true);
            fileHandler.setFormatter(new SimpleFormatter());
            logger.addHandler(fileHandler);
            logger.setLevel(Level.ALL);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
