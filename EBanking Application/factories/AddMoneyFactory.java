package org.poo.cb.factories;

import lombok.AllArgsConstructor;
import org.poo.cb.bank.Bank;
import org.poo.cb.commands.AddMoney;
import org.poo.cb.commands.Command;

import java.util.List;

@AllArgsConstructor

public class AddMoneyFactory implements CommandFactory {
    private final Bank bank;
    @Override
    public Command createCommand(List<String> commandLine) {
        String email = commandLine.get(1);
        String amount = commandLine.get(3);
        String currency = commandLine.get(2);
        return new AddMoney(bank, email, currency, amount);
    }
}
