package TemaTest;

public interface Followable<T> {
    void follow(T o);
    void unfollow(T o);
}
