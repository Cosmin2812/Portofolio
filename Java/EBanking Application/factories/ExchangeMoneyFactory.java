package org.poo.cb.factories;

import lombok.AllArgsConstructor;
import org.poo.cb.bank.Bank;
import org.poo.cb.commands.Command;
import org.poo.cb.commands.ExchangeMoney;

import java.util.List;
@AllArgsConstructor
public class ExchangeMoneyFactory implements CommandFactory {
    private final Bank bank;
    @Override
    public Command createCommand(List<String> commandLine) {
        String email = commandLine.get(1);
        String sourceCurrency = commandLine.get(2);
        String destinationCurrency = commandLine.get(3);
        String amount = commandLine.get(4);
        return new ExchangeMoney(bank, email, sourceCurrency, destinationCurrency, amount);
    }
}
