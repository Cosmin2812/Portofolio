package org.poo.cb.factories;

import lombok.AllArgsConstructor;
import org.poo.cb.bank.Bank;
import org.poo.cb.commands.Command;
import org.poo.cb.commands.CreateUser;

import java.util.ArrayList;
import java.util.List;
@AllArgsConstructor
public class CreateUserFactory implements CommandFactory {
    private final Bank bank;

    private List<String> parseArgs(List<String> args) {
        List<String> newArgs = new ArrayList<>();
        newArgs.add(args.get(1)); // email
        newArgs.add(args.get(2)); // firstName
        newArgs.add(args.get(3)); // lastName
        StringBuilder address = new StringBuilder(args.get(4));
        for(int i = 5; i < args.size(); i++) {
            address.append(" ").append(args.get(i));
        }
        newArgs.add(address.toString()); // address
        return newArgs;
    }


    @Override
    public Command createCommand(List<String> args) {
        args = parseArgs(args);
        String email = args.get(0);
        String firstName = args.get(1);
        String lastName = args.get(2);
        String address = args.get(3);
        return new CreateUser(bank, email, firstName, lastName, address);
    }
}
