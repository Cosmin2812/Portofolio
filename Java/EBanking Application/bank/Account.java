package org.poo.cb.bank;

import lombok.Getter;

public class Account {
    private final User owner;
    private @Getter final ValueType type;
    private @Getter float sum;

    public Account(User owner, ValueType type) {
        this.owner = owner;
        this.type = type;
        sum = 0;
    }

    public void addMoney(float amount) {
        sum += amount;
        owner.receiveNotification("Added " + amount + " to account " + type);
    }
    public void removeMoney(float amount) {
        sum -= amount;
        owner.receiveNotification("Removed " + amount + " from account " + type);
    }

}
