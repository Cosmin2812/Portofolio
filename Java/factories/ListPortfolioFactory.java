package org.poo.cb.factories;

import lombok.AllArgsConstructor;
import org.poo.cb.bank.Bank;
import org.poo.cb.commands.Command;
import org.poo.cb.commands.ListPortfolio;

import java.util.List;
@AllArgsConstructor
public class ListPortfolioFactory implements CommandFactory {
    private final Bank bank;
    @Override
    public Command createCommand(List<String> args) {
        String email = args.get(1);
        return new ListPortfolio(bank, email);
    }
}
