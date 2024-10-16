package org.poo.cb.commands;

import lombok.AllArgsConstructor;
import org.poo.cb.bank.Bank;
import org.poo.cb.bank.User;
import org.poo.cb.bank.ValueType;

@AllArgsConstructor
public class AddMoney implements Command {
    private Bank bank;
    private String email;
    private String valueType;
    private String amount;

    @Override
    public void execute() {
        // Consider that user exists and has an account of type valueType
        User user = bank.findUser(email);
        ValueType currency = ValueType.valueOf(valueType);
        bank.addMoney(user, currency, currency, Float.parseFloat(amount));
    }

}
