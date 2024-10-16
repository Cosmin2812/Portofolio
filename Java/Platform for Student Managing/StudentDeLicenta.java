package org.example;

import java.util.ArrayList;

public class StudentDeLicenta extends Student {
    public StudentDeLicenta(String nume) {
        super(nume);
    }
    private ArrayList<Curs<StudentDeLicenta>> preferinteCursuri = new ArrayList<>();
}
