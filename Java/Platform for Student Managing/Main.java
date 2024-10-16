package org.example;

import java.io.*;
import java.util.Arrays;
import java.util.Scanner;

public class Main {

    public static String antetResources = "src/main/resources/";

    public static Scanner getScanner(String path) throws IOException {
        File inputData = new File(path);
        return new Scanner(inputData);
    }

    /* parse command to a string array */
    public static String[] getCommandLine(Scanner s) throws IOException {
        if(s.hasNextLine()) {
            String line = s.nextLine();
            return line.split(" - ");
        }
        return null;
    }

    public static PrintWriter getPrintWriter(String path) throws IOException {
            FileWriter fw = new FileWriter(path, true);
            BufferedWriter bw = new BufferedWriter(fw);
            return new PrintWriter(bw);
    }

    public static void main(String[] args) {
        Secretariat secretariat = new Secretariat();
        String inPath = antetResources + args[0] + "/" + args[0] + ".in";
        String outPath = antetResources + args[0] + "/" + args[0] + ".out";

        try {
            Scanner s = getScanner(inPath);
            String[] commandLine = getCommandLine(s);

            while (commandLine != null) {
                String command = commandLine[0];
                try {
                    switch (command) {
                        case "adauga_student":
                            secretariat.adaugaStudent(commandLine[2], commandLine[1]);
                            break;
                        case "citeste_mediile":
                            String pathNote = antetResources + args[0];
                            secretariat.citesteMedii(pathNote);
                            break;
                        case "posteaza_mediile":
                            secretariat.posteazaMedii(getPrintWriter(outPath));
                            break;
                        case "contestatie":
                            secretariat.contestatie(commandLine[1], Double.parseDouble(commandLine[2]));
                            break;
                        case "adauga_preferinte":
                            String[] preferinte = Arrays.copyOfRange(commandLine, 2, commandLine.length);
                            secretariat.adaugaPreferinte(commandLine[1], preferinte);
                            break;
                        case "adauga_curs":
                            secretariat.adaugaCurs(commandLine[2], commandLine[1], Integer.parseInt(commandLine[3]));
                            break;
                        case "repartizeaza":
                            secretariat.repartizare("licenta");
                            secretariat.repartizare("master");
                            break;
                        case "posteaza_curs":
                            Curs<? extends Student> curs = secretariat.searchCurs(commandLine[1]);
                            if (curs == null) {
                                throw new Exception("Cursul " + commandLine[1] + " nu exista");
                            }
                            curs.afiseazaCurs(getPrintWriter(outPath));
                            break;
                        case "posteaza_student":
                            Student student = secretariat.searchStudent(commandLine[1]);
                            if (student == null) {
                                throw new Exception("Studentul " + commandLine[1] + " nu exista");
                            }
                            student.afiseazaStudent(getPrintWriter(outPath));
                            break;
                    }
                } catch (ExceptieStudentDuplicat e) {
                    PrintWriter out = getPrintWriter(outPath);
                    out.println("***");
                    out.println(e.getMessage());
                    out.close();
                }
                commandLine = getCommandLine(s);
            }
        } catch (IOException e) {
            System.out.println("Eroare la citirea din fisier");
        } catch (Exception e) {
            System.out.println(e.getMessage());
            e.printStackTrace();
        }



    }
}
