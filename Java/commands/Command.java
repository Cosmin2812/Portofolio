package org.poo.cb.commands;

import java.util.List;

public interface Command {
    void execute();
    default void sendError(String message) {
        System.out.println(message);
    }
}
