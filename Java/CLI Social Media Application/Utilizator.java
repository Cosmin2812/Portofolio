package TemaTest;

import java.io.*;
import java.util.*;
import java.util.Scanner;
import java.util.StringTokenizer;

public class Utilizator implements Followable<Utilizator> {
    private static int numberOfUsers = 0;
    private int id;
    private String username;
    private String password;
    private ArrayList<Postare> posts;
    private ArrayList<Utilizator> follows;
    private ArrayList<Utilizator> isFollowedBy;
    private ArrayList<Postare> likedPosts;
    private ArrayList<Comentariu> likedComments;
    private ArrayList<Comentariu> writtenComments;

    public Utilizator() {
        posts = new ArrayList<>();
        follows = new ArrayList<>();
        isFollowedBy = new ArrayList<>();
        likedPosts = new ArrayList<>();
        likedComments = new ArrayList<>();
        writtenComments = new ArrayList<>();
    };

    public Utilizator(String username, String password) {
        this.username = username;
        this.password = password;
        numberOfUsers++;
        this.id = numberOfUsers;
        posts = new ArrayList<>();
        follows = new ArrayList<>();
        isFollowedBy = new ArrayList<>();
        likedPosts = new ArrayList<>();
        writtenComments = new ArrayList<>();
    }

    /* Method used when reading a User from a file */
    /* The numberOfUsers doesn't get changed, */
    /* so the next created users will have the proper ids */
    public void recreateUser(String username, String password, int id) {
        this.username = username;
        this.password = password;
        this.id = id;

    }

    public Postare getPost(int id) {
        for(int i = 0; i < posts.size(); i++) {
            if(posts.get(i).getId() == id) {
                return posts.get(i);
            }
        }
        return null;
    }

    public int getId() {
        return id;
    }

    public String getUsername() {
        return username;
    }

    public String getPassword() {
        return password;
    }

    public ArrayList<Postare> getPosts() {
        return posts;
    }

    public ArrayList<Utilizator> getFollows() {
        return follows;
    }

    public ArrayList<Utilizator> getIsFollowedBy() {
        return isFollowedBy;
    }

    public ArrayList<Comentariu> getWrittenComments() {
        return writtenComments;
    }

    public int getNumberOfLikes() {
        int postLikes = 0, commentLikes = 0;
        for(int i = 0; i < posts.size(); i++) {
            postLikes += posts.get(i).getLikedByUsers().size();
        }
        for(int i = 0; i < writtenComments.size(); i++) {
            commentLikes += writtenComments.get(i).getLikedByUsers().size();
        }
        return postLikes + commentLikes;
    }

    public void removePost(Postare p) {
        posts.remove(p);
    }

    public static void resetNumberOfUsers() {
        numberOfUsers = 0;
    }

    Postare addPost(Postare p) {
        posts.add(p);
        return p;
    }

    public boolean cmpPassword(String passTry) {
        return passTry.equals(password);
    }

    public boolean FollowsUser(Utilizator u) {
        for(int i = 0; i < follows.size(); i++) {
            if(follows.get(i).getId() == u.getId()) {
                return true;
            }
        }
        return false;
    }

    public void likePost(Postare p) {
        likedPosts.add(p);
    }

    public void unlikePost(Postare p) {
        likedPosts.remove(p);
    }

    public void likeComment(Comentariu comentariu) {
        likedComments.add(comentariu);
    }

    public void unlikeComment(Comentariu comment) {
        likedComments.remove(comment);
    }

    public void follow(Utilizator u) {
        this.follows.add(u);
        u.isFollowedBy.add(this);
    }

    public void unfollow(Utilizator u) {
        this.follows.remove(u);
        u.isFollowedBy.remove(this);
    }

    public Comentariu findCommentById(int id) {
        for(int i = 0; i < writtenComments.size(); i++) {
            if(writtenComments.get(i).getId() == id) {
                return writtenComments.get(i);
            }
        }
        return null;
    }

    /* Two classes containing Comparator Methods */
    /* Used when trying to sort an Array of Users */
    public static class ReverseFollowComparator implements Comparator<Utilizator> {
        @Override
        public int compare(Utilizator u1, Utilizator u2) {
            return u2.isFollowedBy.size() - u1.isFollowedBy.size();
        }
    }

    public static class ReverseLikesComparator implements Comparator<Utilizator> {
        @Override
        public int compare(Utilizator u1, Utilizator u2) {
            return u2.getNumberOfLikes() - u1.getNumberOfLikes();
        }
    }
}
