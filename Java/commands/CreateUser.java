package org.poo.cb.commands;

import lombok.AllArgsConstructor;
import org.poo.cb.bank.Bank;
import org.poo.cb.bank.User;

@AllArgsConstructor
public class CreateUser implements Command {
    private Bank myBank;
    private String email;
    private String firstName;
    private String lastName;
    private String address;

    public void execute() {
        User user = myBank.findUser(email);
        if(user != null) {
            sendError("User with " + email + " already exists");
            return;
        }

        User newUser = myBank.createUser(email, firstName, lastName, address);
        myBank.addUser(newUser);
    }
}
