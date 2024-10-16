package org.poo.cb.commands;

import lombok.AllArgsConstructor;
import org.poo.cb.bank.Bank;
import org.poo.cb.bank.User;

@AllArgsConstructor
public class BuyStocks implements Command {
    private Bank bank;
    private String email;
    private String companyName;
    private int noOfStocks;


    @Override
    public void execute() {
        User user = bank.findUser(email);
        // Consider that user exists and has an account of type valueType\
        if(!bank.isStocksTransactionValid(user, companyName, noOfStocks)) {
            sendError("Insufficient amount in account for buying stock");
            return;
        }

        bank.buyStocks(user, companyName, noOfStocks);
    }
}
