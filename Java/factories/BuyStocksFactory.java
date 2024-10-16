package org.poo.cb.factories;

import lombok.AllArgsConstructor;
import org.poo.cb.bank.Bank;
import org.poo.cb.commands.BuyStocks;
import org.poo.cb.commands.Command;

import java.util.List;
@AllArgsConstructor
public class BuyStocksFactory implements CommandFactory {
    private final Bank bank;
    public Command createCommand(List<String> args) {
        String email = args.get(1);
        String companyName = args.get(2);
        int noOfStocks = Integer.parseInt(args.get(3));
        return new BuyStocks(bank, email, companyName, noOfStocks);
    }
}
