package org.poo.cb.commands;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import lombok.AllArgsConstructor;
import org.poo.cb.bank.Account;
import org.poo.cb.bank.Bank;
import org.poo.cb.bank.User;
import org.poo.cb.bank.ValueType;

import java.util.*;
@AllArgsConstructor
public class ListPortfolio implements Command {
    private final Bank bank;
    private final String email;
    @Override
    public void execute() {
        User user = bank.findUser(email);
        if(user == null) {
            sendError("User with " + email + " does not exist");
            return;
        }

        Set<String> companies = bank.getStocks(user).keySet();
        ArrayList<LinkedHashMap<String, Object>> stocksList = new ArrayList<>();
        for(String company : companies) {
            int amount = bank.getStocks(user).get(company);
            stocksList.add(getStockMap(company, amount));
        }
        ArrayList<Account> accounts = bank.getAccounts(user);
        ArrayList<LinkedHashMap<String, Object>> accountsList = new ArrayList<>();
        for(Account account : accounts) {
            ValueType currency = account.getType();
            float amount = account.getSum();
            accountsList.add(getAccountMap(user, currency, amount));
        }

        Map<String, Object> portofolioMap = new LinkedHashMap<>();
        portofolioMap.put("stocks", stocksList);
        portofolioMap.put("accounts", accountsList);

        /* Print portfolio  in JSON format */
        ObjectMapper objectMapper = new ObjectMapper();
        JsonNode userJSON = objectMapper.valueToTree(portofolioMap);
        String jsonString = userJSON.toString();
        System.out.println(jsonString);
    }

    private static LinkedHashMap<String, Object> getStockMap(String company, int amount) {
        LinkedHashMap<String, Object> stockMap = new LinkedHashMap<>();
        stockMap.put("stockName", company);
        stockMap.put("amount", amount);
        return stockMap;
    }

    private static LinkedHashMap<String, Object> getAccountMap(User user, ValueType currency, float amount) {
        LinkedHashMap<String, Object> accountMap = new LinkedHashMap<>();
        accountMap.put("currencyName", currency);
        accountMap.put("amount", String.format("%.02f", amount));
        return accountMap;
    }
}
