package org.poo.cb.bank;

import lombok.Getter;
import org.poo.cb.utils.Logger;

import java.util.ArrayList;
import java.util.LinkedHashMap;

public class NormalUser implements User {
    private final Bank bank;
    private final @Getter String firstName;
    private final @Getter String lastName;
    private final @Getter String email;
    private final @Getter String address;
    private final @Getter ArrayList<User> friends = new ArrayList<User>();
    private final @Getter LinkedHashMap<String, Integer> stocks = new LinkedHashMap<>();
    private final @Getter ArrayList<Account> accounts = new ArrayList<Account>();

    public NormalUser(Bank bank, String email, String firstName, String lastName, String address) {
        this.bank = bank;
        this.email = email;
        this.firstName = firstName;
        this.lastName = lastName;
        this.address = address;
    }

    @Override
    public void buyStocks(String companyName, int noOfStocks, Account account, float price, ArrayList<String> recommendedStocks) {
        account.removeMoney(price);
        getStocks().merge(companyName, noOfStocks, Integer::sum);
    }

    @Override
    public void removeMoney(float exchangeRate, Account sourceAccount, boolean exchange, float amount) {
        if(!exchange) {
            sourceAccount.removeMoney(amount * exchangeRate);
            return;
        }
        float errand = 0;
        if(amount * exchangeRate > 0.5 * sourceAccount.getSum()) {
            errand = amount / 100;
        }
        sourceAccount.removeMoney((amount + errand) * exchangeRate);
    }

    @Override
    public User findFriend(String email) {
        for (User friend : friends) {
            if (friend.getEmail().equals(email)) {
                return friend;
            }
        }
        return null;
    }

    @Override
    public void receiveNotification(String message) {
        Logger.getInstace().info("User " + email + " received notification: " + message);
    }

    @Override
    public void addAccount(ValueType accountType) {
        accounts.add(new Account(this, accountType));
    }

    @Override
    public Account findAccount(ValueType accountType) {
        for (Account account : accounts) {
            if (account.getType().equals(accountType)) {
                return account;
            }
        }
        return null;
    }

}
