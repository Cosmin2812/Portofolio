package org.poo.cb.commands;

import lombok.AllArgsConstructor;
import org.poo.cb.bank.Bank;
import org.poo.cb.bank.User;

@AllArgsConstructor
public class BuyPremium implements Command {
    private Bank bank;
    private String email;

    @Override
    public void execute() {
        User user = bank.findUser(email);
        if(!bank.isPremiumTransactionValid(user)) {
            sendError("Insufficient amount in account for buying premium option");
            return;
        }

        bank.buyPremium(user);
    }
}
