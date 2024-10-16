package org.poo.cb.commands;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import lombok.AllArgsConstructor;
import org.poo.cb.bank.Bank;

import java.util.ArrayList;
import java.util.Map;

@AllArgsConstructor
public class RecommendStocks implements Command {
    Bank bank;

    @Override
    public void execute() {
        ArrayList<String> recommendedStocks = bank.getRecommendedStocks();

        /* Print recommended stocks in JSON format */
        Map<String, Object> response = Map.of("stocksToBuy", recommendedStocks);
        ObjectMapper objectMapper = new ObjectMapper();
        JsonNode userJSON = objectMapper.valueToTree(response);
        String jsonString = userJSON.toString();
        System.out.println(jsonString);
    }

}
