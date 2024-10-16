package org.poo.cb.utils;

import com.opencsv.CSVReader;
import com.opencsv.exceptions.CsvException;
import org.poo.cb.bank.ValueType;

import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Scanner;

public class FileHandlerBank {
    public static List<String[]> parseInputFileTXT(String fileName) throws IOException {
        File file = new File(fileName);
        Scanner sc = new Scanner(file);
        List<String[]> commands = new ArrayList<>();
        while (sc.hasNextLine()) {
            String line = sc.nextLine();
            String[] command = line.split(" ");
            commands.add(command);
        }
        return commands;
    }

    public static HashMap<ValueType, HashMap<ValueType, Float>> getExchangeRates(String fileName) throws CsvException, IOException {
        CSVReader reader = new CSVReader(new FileReader(fileName));
        List<String[]> lines = reader.readAll();
        lines.remove(0);
        HashMap<ValueType, HashMap<ValueType, Float>> exchangeRates = new HashMap<>();
        for (String[] line : lines) {
            HashMap<ValueType, Float> rates = new HashMap<ValueType, Float>();
            ValueType valueType = ValueType.valueOf(line[0]);
            String[] currencies = {"EUR", "GBP", "JPY", "CAD", "USD"};
            for (int i = 1; i < line.length; i++) {
                rates.put(ValueType.valueOf(currencies[i - 1]), Float.parseFloat(line[i]));
            }
            exchangeRates.put(ValueType.valueOf(line[0]), rates);
        }
        return exchangeRates;
    }

    public static HashMap<String, ArrayList<Float>> getStocks(String fileName) throws IOException, CsvException {
        CSVReader reader = new CSVReader(new FileReader(fileName));
        List<String[]> lines = reader.readAll();
        lines.remove(0);
        HashMap<String, ArrayList<Float>> stocks = new HashMap<>();
        for(String[] line : lines) {
            String companyName = line[0];
            ArrayList<Float> values = new ArrayList<>();
            for(int i = 1; i < line.length; i++) {
                values.add(Float.parseFloat(line[i]));
            }
            stocks.put(companyName, values);
        }
        return stocks;
    }
}
