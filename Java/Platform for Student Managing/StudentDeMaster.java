package org.example;

import java.util.ArrayList;

public class StudentDeMaster extends Student {
    public StudentDeMaster(String nume) {
        super(nume);
    }
    private ArrayList<Curs<StudentDeMaster>> preferinteCursuri = new ArrayList<>();
}
