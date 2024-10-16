package org.poo.cb.factories;

import lombok.AllArgsConstructor;
import org.poo.cb.bank.Bank;
import org.poo.cb.commands.BuyPremium;
import org.poo.cb.commands.Command;

import java.util.List;

@AllArgsConstructor
public class BuyPremiumFactory implements CommandFactory {
    private final Bank bank;
    @Override
    public Command createCommand(List<String> commandLine) {
        String email = commandLine.get(1);
        return new BuyPremium(bank, email);
    }
}
