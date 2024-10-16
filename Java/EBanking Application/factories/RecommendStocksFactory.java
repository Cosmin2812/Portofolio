package org.poo.cb.factories;

import lombok.AllArgsConstructor;
import org.poo.cb.bank.Bank;
import org.poo.cb.commands.Command;
import org.poo.cb.commands.RecommendStocks;

import java.util.List;
@AllArgsConstructor
public class RecommendStocksFactory implements CommandFactory {
    private final Bank bank;
    public Command createCommand(List<String> args) {
        return new RecommendStocks(bank);
    }
}
