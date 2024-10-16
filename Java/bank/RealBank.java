package org.poo.cb.bank;

import lombok.Setter;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Set;

public class RealBank implements Bank {
    private static RealBank instance = null;
    private String name;
    private final ArrayList<User> users;
    private @Setter HashMap<ValueType, HashMap<ValueType, Float>> exchangeRates;
    private @Setter HashMap<String, ArrayList<Float>> stocks;
    private ArrayList<String> recommendedStocks;

    private RealBank() {
        readAttributes();
        users = new ArrayList<>();
        exchangeRates = new HashMap<>();
        stocks = new HashMap<>();
        recommendedStocks = new ArrayList<>();
    }

    private void readAttributes() {
        name = "eBank";
    }

    public ArrayList<String> getRecommendedStocks() {
        recommendedStocks = recommendStocks();
        return recommendedStocks;
    }

    public static RealBank getInstance() {
        if (instance == null) {
            instance = new RealBank();
        }
        return instance;
    }

    public User findUser(String email) {
        for(User user : users) {
            if(user.getEmail().equals(email)) {
                return user;
            }
        }
        return null;
    }

    public void addUser(User user) {
        users.add(user);
    }

    public float getExchangeRate(ValueType source, ValueType destination) {
        return exchangeRates.get(source).get(destination);
    }

    public Float getStockValue(String companyName) {
        ArrayList<Float> valuesOverTime = stocks.get(companyName);
        return stocks.get(companyName).get(valuesOverTime.size() - 1);
    }

    public void buyStocks(User user, String companyName, int noOfStocks) {
        if(recommendedStocks == null) {
            recommendedStocks = getRecommendedStocks();
        }
        Account account = user.findAccount(ValueType.USD);
        float price = noOfStocks * getStockValue(companyName);
        user.buyStocks(companyName, noOfStocks, account, price, recommendedStocks);
    }

    public float getSMA(String companyName, int days) {
        ArrayList<Float> valuesOverTime = stocks.get(companyName);
        int sum = 0;
        for(int i = valuesOverTime.size() - 1; i >= valuesOverTime.size() - days; i--) {
            sum += valuesOverTime.get(i);
        }
        return (float) sum / days;
    }

    public ArrayList<String> recommendStocks() {
        ArrayList<String> recommendedStocks = new ArrayList<String>();
        Set<String> companies = stocks.keySet();
        for(String company : companies) {
            float fiveDaySMA = getSMA(company, 5);
            float tenDaySMA = getSMA(company, 10);
            if(fiveDaySMA > tenDaySMA) {
                recommendedStocks.add(company);
            }
        }
        return recommendedStocks;
    }

    public User createUser(String email, String firstName, String lastName, String address) {
        return new NormalUser(this, email, firstName, lastName, address);
    }

    public void addFriend(User user, User friend) {
        user.getFriends().add(friend);
    }

    public Account findAccount(User user, ValueType valueType) {
        return user.findAccount(valueType);
    }

    public void addAccount(User user, ValueType valueType) {
        user.addAccount(valueType);
    }

    public void removeMoney(User user, ValueType source, ValueType destination, boolean exchange, float amount) {
        Account sourceAccount = user.findAccount(source);
        float exchangeRate = getExchangeRate(destination, source);
        user.removeMoney(exchangeRate, sourceAccount, exchange, amount);
    }

    public void addMoney(User user, ValueType sourceValueType, ValueType destinationValueType, float transferAmount) {
        Account destinationAccount = user.findAccount(destinationValueType);
//        float exchangeRate = getExchangeRate(sourceValueType, destinationValueType);
        destinationAccount.addMoney(transferAmount);

    }
    public HashMap<String, Integer> getStocks(User user) {
        return user.getStocks();
    }

    public ArrayList<Account> getAccounts(User user) {
        return user.getAccounts();
    }

    public boolean isTransferAcceptable(User user, ValueType sourceValueType, ValueType destinationValueType, float transferAmount) {
        Account sourceAccount = user.findAccount(sourceValueType);
        float exchangeRate = getExchangeRate(destinationValueType, sourceValueType);
        float errand = 0;
        if(transferAmount * exchangeRate > 0.5 * sourceAccount.getSum()) {
            errand = transferAmount / 100;
        }
        return sourceAccount.getSum() >= (transferAmount + errand) * exchangeRate;
    }

    public boolean isStocksTransactionValid(User user, String companyName, int noOfStocks) {
        Account account = user.findAccount(ValueType.USD);
        float price = noOfStocks * getStockValue(companyName);
        return account.getSum() >= price;
    }

    public static void destroy() {
        instance = null;
    }

    public boolean isPremiumTransactionValid(User user) {
        Account account = user.findAccount(ValueType.USD);
        return account.getSum() >= 100;
    }

    public void buyPremium(User user) {
        Account account = user.findAccount(ValueType.USD);
        account.removeMoney(100);
        users.remove(user);
        users.add(new PremiumUser((NormalUser) user));
    }
}
