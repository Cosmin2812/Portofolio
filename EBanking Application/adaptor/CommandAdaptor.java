package org.poo.cb.adaptor;

import org.poo.cb.bank.Bank;
import org.poo.cb.commands.Command;
import org.poo.cb.factories.*;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;

public class CommandAdaptor {
    private final HashMap<String, CommandFactory> stringToClassCommand;

    public CommandAdaptor(Bank bankProxy) {
        stringToClassCommand = new HashMap<>();
        stringToClassCommand.put("CREATE USER", new CreateUserFactory(bankProxy));
        stringToClassCommand.put("LIST USER", new ListUserFactory(bankProxy));
        stringToClassCommand.put("ADD FRIEND", new AddFriendFactory(bankProxy));
        stringToClassCommand.put("ADD ACCOUNT", new AddAccountFactory(bankProxy));
        stringToClassCommand.put("ADD MONEY", new AddMoneyFactory(bankProxy));
        stringToClassCommand.put("EXCHANGE MONEY", new ExchangeMoneyFactory(bankProxy));
        stringToClassCommand.put("RECOMMEND STOCKS", new RecommendStocksFactory(bankProxy));
        stringToClassCommand.put("LIST PORTFOLIO", new ListPortfolioFactory(bankProxy));
        stringToClassCommand.put("TRANSFER MONEY", new TransferMoneyFactory(bankProxy));
        stringToClassCommand.put("BUY STOCKS", new BuyStocksFactory(bankProxy));
        stringToClassCommand.put("BUY PREMIUM", new BuyPremiumFactory(bankProxy));
    }

    public List<String> parseCommand(String[] commandLine) {
        String command = commandLine[0] + " " + commandLine[1];
        List<String> commandList = new ArrayList<String>(Arrays.asList(commandLine));
        commandList.remove(0);
        commandList.remove(0);
        commandList.add(0, command);
        return commandList;
    }

    public void executeCommand(String[] input) {
        List<String> commandLine = parseCommand(input);
        String commandName = commandLine.get(0);
        CommandFactory commandFactory = stringToClassCommand.get(commandName);

        if(commandFactory == null) {
            System.out.println("Command not found");
            return;
        }

        Command command = commandFactory.createCommand(commandLine);
        command.execute();
    }
}
