package org.example;

import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.Comparator;

public class Student {
    private String nume;
    private double medie;
    private ArrayList<Curs<? extends Student>> preferinteCursuri;
    private Curs<? extends Student> cursAsignat;

    public Student(String nume) {
        this.nume = nume;
        this.preferinteCursuri = new ArrayList<>();
    }

    public String getNume() {
        return nume;
    }

    public void setNume(String nume) {
        this.nume = nume;
    }

    public double getMedie() {
        return medie;
    }

    public void setMedie(double medie) {
        this.medie = medie;
    }

    public void addPreferinta(Curs<? extends Student> curs) {
        preferinteCursuri.add(curs);
    }

    public ArrayList<Curs<? extends Student>> getPreferinteCursuri() {
        return preferinteCursuri;
    }

    public static class ReverseMedieComparator implements Comparator<Student> {
        @Override
        public int compare(Student s1, Student s2) {
            if(s1.getMedie() == s2.getMedie())
                return s1.getNume().compareTo(s2.getNume());
            return Double.compare(s2.getMedie(), s1.getMedie());
        }
    }

    public static class NumeComparator implements Comparator<Student> {
        @Override
        public int compare(Student s1, Student s2) {
            return s1.getNume().compareTo(s2.getNume());
        }
    }

    public void afiseazaStudent(PrintWriter out) {
        out.println("***");
        out.print("Student ");
        if(this instanceof StudentDeLicenta) {
            out.print("Licenta: ");
        } else {
            out.print("Master: ");
        }
        out.println(this.nume + " - " + this.medie + " - " + this.cursAsignat.getNume());
        out.close();
    }

    public void setCursAsignat(Curs<? extends Student> cursAsignat) {
        this.cursAsignat = cursAsignat;
    }
}
