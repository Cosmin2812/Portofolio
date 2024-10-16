package org.example;

import java.io.*;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Scanner;

/* exception class for duplicate student case */
class ExceptieStudentDuplicat extends Exception {
    public ExceptieStudentDuplicat(String message) {
        super(message);
    }
}

public class Secretariat {
    private ArrayList<Curs<StudentDeLicenta>> cursuriLicenta;
    private ArrayList<Curs<StudentDeMaster>> cursuriMaster;
    private HashMap<String, ArrayList<Student>> studenti;

    public Secretariat() {
        this.cursuriLicenta = new ArrayList<>();
        this.cursuriMaster = new ArrayList<>();
        this.studenti = new HashMap<>();
    }

    public Student searchStudent(String nume, String cicluDeStudii) {
        for(Student student : studenti.get(cicluDeStudii)) {
            if(student.getNume().equals(nume)) {
                return student;
            }
        }
        return null;
    }

    /* overload method in case cicluDeStudii is unknown */
    public Student searchStudent(String nume) {
        for(ArrayList<Student> studenti : studenti.values()) {
            for(Student student : studenti) {
                if(student.getNume().equals(nume)) {
                    return student;
                }
            }
        }
        return null;
    }

    public Curs<? extends Student> searchCurs(String numeCurs, String cicluDeStudii) {
        if(cicluDeStudii.equals("licenta")) {
            for(Curs<StudentDeLicenta> curs : cursuriLicenta) {
                if(curs.getNume().equals(numeCurs)) {
                    return curs;
                }
            }
        } else if (cicluDeStudii.equals("master")) {
            for(Curs<StudentDeMaster> cursMaster : cursuriMaster) {
                if(cursMaster.getNume().equals(numeCurs)) {
                    return cursMaster;
                }
            }
        }
        return null;
    }
    
    public Curs<? extends Student> searchCurs(String numeCurs) {
        for(Curs<StudentDeLicenta> curs : cursuriLicenta) {
            if(curs.getNume().equals(numeCurs)) {
                return curs;
            }
        }
        for(Curs<StudentDeMaster> cursMaster : cursuriMaster) {
            if(cursMaster.getNume().equals(numeCurs)) {
                return cursMaster;
            }
        }
        return null;
    }

    public void adaugaStudent(String nume, String cicluDeStudii) throws Exception {
        if(searchStudent(nume) != null) {
            throw new ExceptieStudentDuplicat("Student duplicat: " + nume);
        }

        if(!studenti.containsKey(cicluDeStudii)) {
            if(cicluDeStudii.equals("licenta")) {
                studenti.put(cicluDeStudii, new ArrayList<>());
            } else if (cicluDeStudii.equals("master")) {
                studenti.put(cicluDeStudii, new ArrayList<>());
            }
        }

        if(cicluDeStudii.equals("licenta")) {
            studenti.get(cicluDeStudii).add(new StudentDeLicenta(nume));
        } else if (cicluDeStudii.equals("master")) {
            studenti.get(cicluDeStudii).add(new StudentDeMaster(nume));
        } else {
            throw new Exception("Ciclu de studii invalid: " + cicluDeStudii);
        }
    }

    public void adaugaCurs(String numeCurs, String cicluDeStudii, int capacitateMax) throws Exception {
        if(cicluDeStudii.equals("licenta")) {
            cursuriLicenta.add(new Curs<>(numeCurs, capacitateMax));
        } else if (cicluDeStudii.equals("master")) {
            cursuriMaster.add(new Curs<>(numeCurs, capacitateMax));
        } else {
            throw new Exception("Ciclu de studii invalid: " + cicluDeStudii);
        }
    }

    public void citesteMedii(String path) throws ExceptieStudentDuplicat {
        int i = 1;
        while(true) {
            try {
                File file = new File(path + "/note_" + i + ".txt");
                if(file.exists())
                    i++;
                else
                    break;

                Scanner s = new Scanner(file);
                while(s.hasNextLine()) {
                    String line = s.nextLine();
                    String[] tokens = line.split(" - ");
                    String nume = tokens[0];

                    Student student = searchStudent(nume);
                    if (student == null) {
                        throw new ExceptieStudentDuplicat("Studentul " + nume + " nu exista");
                    }

                    double medie = Double.parseDouble(tokens[1]);
                    student.setMedie(medie);
                }
                s.close();

            } catch (FileNotFoundException e) {
                System.out.println("Fisierul nu exista");
            }
        }
    }

    public void posteazaMedii(PrintWriter out) {
        /* sort the students first and then print the info */
        ArrayList<Student> allStudenti = new ArrayList<>();
        if(studenti.containsKey("licenta"))
            allStudenti.addAll(studenti.get("licenta"));
        if(studenti.containsKey("master"))
            allStudenti.addAll(studenti.get("master"));
        allStudenti.sort(new Student.ReverseMedieComparator());

        out.println("***");
        for(Student student : allStudenti) {
            out.println(student.getNume() + " - " + student.getMedie());
        }
        out.close();
    }

    public void contestatie(String nume, double medie) throws Exception {
        Student student = searchStudent(nume);
        if(student == null) {
            throw new Exception("Studentul " + nume + " nu exista");
        }
        student.setMedie(medie);
    }


    public void adaugaPreferinte(String nume, String[] preferinteCurs) {
        Student student = searchStudent(nume, "licenta");
        if(student == null) {
            student = searchStudent(nume, "master");
        }
        if(student == null) {
            System.out.println("Studentul " + nume + " nu exista");
            return;
        }

        for(String curs : preferinteCurs) {
            Curs<? extends Student> cursCautat = searchCurs(curs);
            if(cursCautat == null) {
                System.out.println("Cursul " + curs + " nu exista");
                return;
            }
            student.addPreferinta(cursCautat);
        }
    }

    /* using generalization, so we write only one function for both course types */
    public<T extends Student> void repartizare(String cicluDeStudii) {
        if(!studenti.containsKey(cicluDeStudii))
            return;
        studenti.get(cicluDeStudii).sort(new Student.ReverseMedieComparator());
        for(Student student : studenti.get(cicluDeStudii)) {
            T studentCicluDeStudii = (T) student;
            boolean repartizat = false;
            for(Curs<? extends Student> cursGeneral : studentCicluDeStudii.getPreferinteCursuri()) {
                Curs<T> curs = (Curs<T>) cursGeneral;
                ArrayList<T> studentiInscrisi = curs.getStudentiInscrisi();

                /* if no students are enrolled in the course then simply add studentCicluDeStudii to it */
                /* same if the course is not at full capacity */
                if(studentiInscrisi == null
                        || studentiInscrisi.isEmpty()
                        || curs.getCapacitateMaxima() > studentiInscrisi.size()) {
                    curs.addStudent(studentCicluDeStudii);
                    studentCicluDeStudii.setCursAsignat(curs);
                    repartizat = true;
                    break;
                }

                /* edge case */
                /* add student if he has the same grade as the last one in the course */
                T lastStudent = studentiInscrisi.get(studentiInscrisi.size() - 1);
                if(lastStudent.getMedie() == studentCicluDeStudii.getMedie()) {
                    curs.addStudent(studentCicluDeStudii);
                    studentCicluDeStudii.setCursAsignat(curs);
                    repartizat = true;
                    break;
                }
            }

            /* bonus task */
            /* sort the courses by a chosen criteria */
            /* add student to the first course on the list */
            if(!repartizat) {
                if(studentCicluDeStudii instanceof StudentDeLicenta) {
                    cursuriLicenta.sort(new Curs.CapacitateComparator());
                    cursuriLicenta.get(0).addStudent((StudentDeLicenta) studentCicluDeStudii);
                    studentCicluDeStudii.setCursAsignat(cursuriLicenta.get(0));
                } else if(studentCicluDeStudii instanceof StudentDeMaster) {
                    cursuriMaster.sort(new Curs.CapacitateComparator());
                    cursuriMaster.get(0).addStudent((StudentDeMaster) studentCicluDeStudii);
                    studentCicluDeStudii.setCursAsignat(cursuriMaster.get(0));
                }
            }
        }
    }
}
