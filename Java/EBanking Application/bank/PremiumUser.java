package org.poo.cb.bank;

import lombok.AllArgsConstructor;
import lombok.experimental.Delegate;

import java.util.ArrayList;

@AllArgsConstructor
public class PremiumUser implements User {
    /* The methods that don't get overridden by the delegate */
    private interface Exclude {
        void buyStocks(String companyName, int noOfStocks, Account account, float price, ArrayList<String> recommendedStocks);
        void removeMoney(float exchangeRate, Account sourceAccount, boolean exchange, float amount);
    }

    @Delegate(types = User.class, excludes = Exclude.class)
    private NormalUser delegateUser;

    @Override
    public void removeMoney(float exchangeRate, Account sourceAccount, boolean exchange, float amount) {
        sourceAccount.removeMoney(amount * exchangeRate);
    }

    @Override
    public void buyStocks(String companyName, int noOfStocks, Account account, float price, ArrayList<String> recommendedStocks) {
        if(!recommendedStocks.contains(companyName)) {
            account.removeMoney(price);
        } else {
            float priceReduction = 5 * price / 100;
            account.removeMoney(price - priceReduction);
        }
        getStocks().merge(companyName, noOfStocks, Integer::sum);
    }
}
