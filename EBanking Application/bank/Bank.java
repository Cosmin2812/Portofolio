package org.poo.cb.bank;

import org.poo.cb.utils.Logger;

import java.lang.reflect.InvocationHandler;
import java.lang.reflect.Method;
import java.lang.reflect.Proxy;
import java.util.ArrayList;
import java.util.HashMap;

public interface Bank {
    static Bank getInstance() {
        return (Bank) Proxy.newProxyInstance(Bank.class.getClassLoader(), new Class<?>[] {Bank.class}, new InvocationHandler() {
            @Override
            public Object invoke(Object proxy, Method method, Object[] args) throws Throwable {
                Logger.getInstace().info("Method " + method.getName() + " called with args " + args);
                return method.invoke((RealBank) RealBank.getInstance(), args);
            }
        });
    }

    static void destroy() {
        RealBank.destroy();
    }

    void setExchangeRates(HashMap<ValueType, HashMap<ValueType, Float>> exchangeRates);

    void setStocks(HashMap<String, ArrayList<Float>> stocks);

    User findUser(String email);

    void buyStocks(User user, String companyName, int noOfStocks);

    void addUser(User user);

    float getExchangeRate(ValueType valueType, ValueType valueType1);

    ArrayList<String> getRecommendedStocks();

    User createUser(String email, String firstName, String lastName, String address);

    void addFriend(User user, User friend);

    Account findAccount(User user, ValueType valueType);

    void addAccount(User user, ValueType valueType);

    void removeMoney(User user, ValueType source, ValueType destination, boolean exchange, float amount);

    void addMoney(User user, ValueType sourceValueType, ValueType destinationValueType, float transferAmount);

    HashMap<String, Integer> getStocks(User user);

    ArrayList<Account> getAccounts(User user);

    boolean isTransferAcceptable(User user, ValueType sourceValueType, ValueType destinationValueType, float transferAmount);

    boolean isStocksTransactionValid(User user, String companyName, int noOfStocks);

    boolean isPremiumTransactionValid(User user);

    void buyPremium(User user);
}
