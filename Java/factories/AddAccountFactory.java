package org.poo.cb.factories;

import lombok.AllArgsConstructor;
import org.poo.cb.bank.Bank;
import org.poo.cb.commands.AddAccount;
import org.poo.cb.commands.Command;

import java.util.List;
@AllArgsConstructor
public class AddAccountFactory implements CommandFactory {
    private final Bank bank;
    @Override
    public Command createCommand(List<String> args) {
        String email = args.get(1);
        String currency = args.get(2);
        return new AddAccount(bank, email, currency);
    }
}
