package org.poo.cb.commands;

import lombok.AllArgsConstructor;
import org.poo.cb.bank.Bank;
import org.poo.cb.bank.User;

@AllArgsConstructor
public class AddFriend implements Command {
    private Bank myBank;
    private String email;
    private String friendEmail;

    @Override
    public void execute() {
        User user = myBank.findUser(email);
        if(user == null) {
            sendError("User with " + email + " doesn't exist");
            return;
        }

        User friend = myBank.findUser(friendEmail);
        if(friend == null) {
            sendError("User with " + friendEmail + " doesn't exist");
            return;
        }

        if(user.findFriend(friendEmail) != null) {
            sendError("User with " + friendEmail + " is already a friend");
            return;
        }

        myBank.addFriend(user, friend);
        myBank.addFriend(friend, user);
    }
}
