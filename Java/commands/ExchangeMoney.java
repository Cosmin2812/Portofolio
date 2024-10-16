package org.poo.cb.commands;

import lombok.AllArgsConstructor;
import org.poo.cb.bank.Bank;
import org.poo.cb.bank.User;
import org.poo.cb.bank.ValueType;

@AllArgsConstructor
public class ExchangeMoney implements Command {
    private Bank bank;
    private String email;
    private String sourceCurrency;
    private String destinationCurrency;
    private String amount;

    @Override
    public void execute() {
        // Consider that user exists and has an account of type valueType\
        float transferAmount = Float.parseFloat(amount);
        User user = bank.findUser(email);
        ValueType sourceValueType = ValueType.valueOf(sourceCurrency);
        ValueType destinationValueType = ValueType.valueOf(destinationCurrency);

        if(!bank.isTransferAcceptable(user, sourceValueType, destinationValueType, transferAmount)) {
            sendError("Insufficient amount in account " + sourceValueType + " for exchange");
            return;
        }

        bank.removeMoney(user, sourceValueType, destinationValueType, true, transferAmount);
        bank.addMoney(user, sourceValueType, destinationValueType, transferAmount);
    }
}
