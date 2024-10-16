package TemaTest;

public interface Likeable<T>{
    void likedBy(T o);
    void unlikedBy(T o);
}
