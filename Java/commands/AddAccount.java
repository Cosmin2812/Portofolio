package org.poo.cb.commands;

import lombok.AllArgsConstructor;
import org.poo.cb.bank.Bank;
import org.poo.cb.bank.User;
import org.poo.cb.bank.ValueType;

@AllArgsConstructor
public class AddAccount implements Command {
    private Bank myBank;
    private String email;
    private String accountType;

    @Override
    public void execute() {
        /* Consider that user exists */
        User user = myBank.findUser(email);

        /* Consider that account type is valid */
        if(myBank.findAccount(user, ValueType.valueOf(accountType)) != null) {
            sendError("User with " + email + " already has an account of type " + accountType);
            return;
        }

        myBank.addAccount(user, ValueType.valueOf(accountType));
    }

}
