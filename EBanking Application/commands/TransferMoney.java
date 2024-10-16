package org.poo.cb.commands;

import lombok.AllArgsConstructor;
import org.poo.cb.bank.Account;
import org.poo.cb.bank.Bank;
import org.poo.cb.bank.User;
import org.poo.cb.bank.ValueType;

@AllArgsConstructor
public class TransferMoney implements Command {
    private Bank myBank;
    private String userEmail;
    private String friendEmail;
    private String currency;
    private float amount;

    public void execute() {
        // Consider that both users exist and have an account of type valueType
        User user = myBank.findUser(userEmail);
        ValueType valueType = ValueType.valueOf(currency);
        Account sourceAccount = myBank.findAccount(user, valueType);
        if(sourceAccount.getSum() < amount) {
            sendError("Insufficient amount in account " + sourceAccount.getType() + " for transfer");
            return;
        }

        User friend = user.findFriend(friendEmail);
        if(friend == null) {
            sendError("You are not allowed to transfer money to " + friendEmail);
            return;
        }

        myBank.addMoney(friend, valueType, valueType, amount);
        myBank.removeMoney(user, valueType, valueType, false, amount);
    }

}
