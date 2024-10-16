package org.poo.cb.factories;

import lombok.AllArgsConstructor;
import org.poo.cb.bank.Bank;
import org.poo.cb.commands.Command;
import org.poo.cb.commands.TransferMoney;

import java.util.List;
@AllArgsConstructor
public class TransferMoneyFactory implements CommandFactory {
    private final Bank bank;
    public Command createCommand(List<String> args) {
        String userEmail = args.get(1);
        String friendEmail = args.get(2);
        String currency = args.get(3);
        float amount = Float.parseFloat(args.get(4));
        return new TransferMoney(bank, userEmail, friendEmail, currency, amount);
    }
}
