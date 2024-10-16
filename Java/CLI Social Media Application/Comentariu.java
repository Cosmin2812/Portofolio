package TemaTest;

import java.util.ArrayList;
import java.util.Date;
import java.util.Map;

public class Comentariu implements Likeable<Utilizator>{
    private static int numberOfComments = 0;
    private int id;
    private String text;
    private Utilizator parentUser;
    private String date;
    private Postare parentPost;
    private ArrayList<Utilizator> likedByUsers;

    public static void resetNumberOfComments() {
        numberOfComments = 0;
    }

    public Comentariu(String text, Utilizator userParent, Postare parentPost) {
        this(userParent, parentPost);
        numberOfComments++;
        this.id = numberOfComments;
        this.text = text;
        Date unformattedDate = new Date();
        this.date = App.dateFormat.format(unformattedDate);
    }

    public Comentariu(Utilizator userParent, Postare parentPost) {
        this.parentUser = userParent;
        this.parentPost = parentPost;
        userParent.getWrittenComments().add(this);
        likedByUsers = new ArrayList<>();
    }

    /* Method used when reading a Comment from a file */
    /* The numberOfComments doesn't get changed, */
    /* so the next created Comments will have the proper ids */
    public void recreateComment(String text, int id, String date) {
        this.id = id;
        this.text = text;
        this.date = date;
    }

    public int getId() {
        return id;
    }

    public Utilizator getParentUser() {
        return parentUser;
    }

    public Postare getParentPost() {
        return parentPost;
    }

    public String getText() {
        return text;
    }

    public String getDate() {
        return date;
    }

    public ArrayList<Utilizator> getLikedByUsers() {
        return likedByUsers;
    }

    public void likedBy(Utilizator u) {
        likedByUsers.add(u);
        u.likeComment(this);
    }

    public boolean isLikedBy(Utilizator u) {
        for(int i = 0; i < likedByUsers.size(); i++) {
            if(likedByUsers.get(i).getId() == u.getId()) {
                return true;
            }
        }
        return false;
    }

    public void unlikedBy(Utilizator u) {
        likedByUsers.remove(u);
        u.unlikeComment(this);
    }
}
