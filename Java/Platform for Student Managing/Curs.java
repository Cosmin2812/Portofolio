package org.example;

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.Comparator;

public class Curs <T extends Student> {
    private String nume;
    private int capacitateMaxima;
    private ArrayList<T> studentiInscrisi;

    public Curs(String nume, int capacitateMaxima) {
        this.nume = nume;
        this.capacitateMaxima = capacitateMaxima;
        this.studentiInscrisi = new ArrayList<>();
    }

    public String getNume() {
        return nume;
    }

    public void setNume(String nume) {
        this.nume = nume;
    }

    public int getCapacitateMaxima() {
        return capacitateMaxima;
    }

    public void setCapacitateMaxima(int capacitateMaxima) {
        this.capacitateMaxima = capacitateMaxima;
    }

    public ArrayList<T> getStudentiInscrisi() {
        return studentiInscrisi;
    }

    public void setStudentiInscrisi(ArrayList<T> studentiInscrisi) {
        this.studentiInscrisi = studentiInscrisi;
    }

    public void addStudent(T student) {
        studentiInscrisi.add(student);
    }

    public void increaseCapacity() {
        capacitateMaxima++;
    }

    public void afiseazaCurs(PrintWriter out) {
        out.println("***");
        studentiInscrisi.sort(new Student.NumeComparator());
        out.println(this.nume + " (" + this.capacitateMaxima + ")");
        for(Student student : studentiInscrisi) {
            out.println(student.getNume() + " - " + student.getMedie());
        }
        out.close();
    }

    public static class CapacitateComparator implements Comparator<Curs> {
        public int compare(Curs c1, Curs c2) {
            if(c1.studentiInscrisi.size() == c1.getCapacitateMaxima()) {
                if (c2.studentiInscrisi.size() == c2.getCapacitateMaxima()) {
                    if (c1.getCapacitateMaxima() == c2.getCapacitateMaxima())
                        return c1.getNume().compareTo(c2.getNume());
                    else
                        return Integer.compare(c1.getCapacitateMaxima(), c2.getCapacitateMaxima());
                } else {
                    return 1;
                }
            } else if(c2.studentiInscrisi.size() == c2.getCapacitateMaxima()) {
                return -1;
            } else if(c1.studentiInscrisi.size() == c2.studentiInscrisi.size()) {
                return c1.getNume().compareTo(c2.getNume());
            }
            return c1.studentiInscrisi.size() - c2.studentiInscrisi.size();
        }
    }
}
