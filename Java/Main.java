package org.poo.cb;

import com.opencsv.exceptions.CsvException;
import org.poo.cb.bank.Bank;
import org.poo.cb.adaptor.CommandAdaptor;
import org.poo.cb.utils.FileHandlerBank;

import java.io.IOException;
import java.util.List;

public class Main {
    private static final String folderPath = "./src/main/resources/";
    public static void main(String[] args) {
        /* Sanity check */
        if(args == null || args.length != 3) {
            System.out.println("Running Main");
            return;
        }

        /* Get all the file paths */
        String commandsFile = folderPath + args[2];
        String stockValuesFile = folderPath + args[1];
        String exchangeRatesFile = folderPath + args[0];

        /* Create the bank entity and the command handler */
        /* The proxy acts like a logger */
        Bank bankProxy = Bank.getInstance();
        CommandAdaptor commandHandler = new CommandAdaptor(bankProxy);
        try {
            bankProxy.setExchangeRates(FileHandlerBank.getExchangeRates(exchangeRatesFile));
            bankProxy.setStocks(FileHandlerBank.getStocks(stockValuesFile));

            /* Parse the commands file and execute them */
            List<String[]> commands = FileHandlerBank.parseInputFileTXT(commandsFile);
            for (String[] command : commands) {
                commandHandler.executeCommand(command);
            }
        } catch (CsvException e) {
            System.out.println("Error parsing CSV file");
        } catch (IOException e) {
            System.out.println("Error reading CSV file");
        } finally {
            Bank.destroy();
        }
    }
}