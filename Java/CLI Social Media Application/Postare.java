package TemaTest;

import java.text.ParseException;
import java.util.ArrayList;
import java.util.*;
import java.util.Map;

public class Postare implements Likeable<Utilizator> {
    private static int numberOfPosts = 0;
    ArrayList<Comentariu> comments;
    private int id;
    private String text;
    private Utilizator userParent;
    private String date;
    private ArrayList<Utilizator> likedByUsers;

    public static void resetNumberOfPosts() {
        numberOfPosts = 0;
    }

    public Postare(Utilizator user) {
        this.userParent = user;
        comments = new ArrayList<>();
        this.likedByUsers = new ArrayList<>();
    }

    public Postare(Utilizator user, String text) {
        this(user);
        numberOfPosts++;
        this.id = numberOfPosts;
        this.text = text;
        Date unformattedDate = new Date();
        this.date = App.dateFormat.format(unformattedDate);
    }

    /* Method used when reading a Post from a file */
    /* The numberOfPosts doesn't get changed, */
    /* so the next created posts will have the proper ids */
    public void recreatePost(String text, int id, String date) {
        this.id = id;
        this.text = text;
        this.date = date;
    }

    public String getText() {
        return text;
    }

    public int getId() {
        return id;
    }

    public String getDate() {
        return date;
    }

    public ArrayList<Utilizator> getLikedByUsers() {
        return likedByUsers;
    }

    public void likedBy(Utilizator u) {
        likedByUsers.add(u);
        u.likePost(this);
    }
    public void unlikedBy(Utilizator u) {
        likedByUsers.remove(u);
        u.unlikePost(this);
    }

    public Utilizator getParentUser() {
        return userParent;
    }

    public boolean isLikedBy(Utilizator u) {
        for(int i = 0; i < likedByUsers.size(); i++) {
            if(likedByUsers.get(i).getId() == u.getId()) {
                return true;
            }
        }
        return false;
    }

    public void addComment(Comentariu c) {
        comments.add(c);
    }

    public ArrayList<Comentariu> getComments() {
        return comments;
    }

    public void removeComment(Comentariu c) {
        comments.remove(c);
    }


    /* Three classes containing Comparator Methods */
    /* Used when trying to sort an Array of Posts */
    public static class ReverseLikeComparator implements Comparator<Postare> {
        @Override
        public int compare(Postare p1, Postare p2) {
            return p2.likedByUsers.size() - p1.likedByUsers.size();
        }
    }

    public static class ReverseDateComparator implements Comparator<Postare> {
        @Override
        public int compare(Postare p1, Postare p2) {
            Date date1, date2;

            try {
                date1 = App.dateFormat.parse(p1.date);
                date2 = App.dateFormat.parse(p2.date);
            } catch (ParseException e) {
                throw new RuntimeException(e);
            }
            if(date1.compareTo(date2) == 0)
                return p2.id - p1.id;
            return date2.compareTo(date1);
        }
    }

    public static class ReverseCommentComparator implements Comparator<Postare> {
        @Override
        public int compare(Postare p1, Postare p2) {
            return p2.comments.size() - p1.comments.size();
        }
    }
}
