package org.poo.cb.factories;

import lombok.AllArgsConstructor;
import org.poo.cb.bank.Bank;
import org.poo.cb.commands.AddFriend;
import org.poo.cb.commands.Command;

import java.util.List;
@AllArgsConstructor
public class AddFriendFactory implements CommandFactory {
    private final Bank bank;
    @Override
    public Command createCommand(List<String> args) {
        String userEmail = args.get(1);
        String friendEmail = args.get(2);
        return new AddFriend(bank, userEmail, friendEmail);
    }
}
