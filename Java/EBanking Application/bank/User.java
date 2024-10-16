package org.poo.cb.bank;

import java.util.ArrayList;

public interface User {
    User findFriend(String email);

    void receiveNotification(String message);

    void addAccount(ValueType accountType);

    Account findAccount(ValueType accountType);

    String getFirstName();

    String getLastName();

    String getEmail();

    String getAddress();

    java.util.ArrayList<User> getFriends();

    java.util.LinkedHashMap<String, Integer> getStocks();

    java.util.ArrayList<Account> getAccounts();

    void buyStocks(String companyName, int noOfStocks, Account account, float price, ArrayList<String> recommendedStocks);

    void removeMoney(float exchangeRate, Account sourceAccount, boolean exchange, float amount);
}
