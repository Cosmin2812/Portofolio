package org.poo.cb.factories;

import org.poo.cb.commands.Command;

import java.util.List;

public interface CommandFactory {
    Command createCommand(List<String> args);
}
