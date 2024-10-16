package org.poo.cb.commands;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import lombok.AllArgsConstructor;
import org.jetbrains.annotations.NotNull;
import org.poo.cb.bank.Bank;
import org.poo.cb.bank.User;

import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.Map;

@AllArgsConstructor
public class ListUser implements Command {
    private Bank myBank;
    private String userEmail;

    public void execute() {
        User user = myBank.findUser(userEmail);
        if(user == null) {
            sendError("User with email " + userEmail + " doesn't exist");
            return;
        }

        // Print user information in JSON format
        Map<String, Object> userMap = getUserMap(user);
        ObjectMapper objectMapper = new ObjectMapper();
        JsonNode userJSON = objectMapper.valueToTree(userMap);

        String jsonString = userJSON.toString();
        System.out.println(jsonString);
    }

    @NotNull
    private static Map<String, Object> getUserMap(User user) {
        Map<String, Object> userMap = new LinkedHashMap<>();
        userMap.put("email", user.getEmail());
        userMap.put("firstname", user.getFirstName());
        userMap.put("lastname", user.getLastName());
        userMap.put("address", user.getAddress());
        ArrayList<String> friendsEmails = new ArrayList<String>();
        for(User friend : user.getFriends()) {
            friendsEmails.add(friend.getEmail());
        }
        userMap.put("friends", friendsEmails);
        return userMap;
    }
}
