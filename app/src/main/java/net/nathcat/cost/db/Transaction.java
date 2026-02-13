package net.nathcat.cost.db;

import net.nathcat.sql.DBType;

public class Transaction implements DBType {
  public int id;
  public int payer;
  public int payeeCount;
  public int amount;
  public int group;
  public long timestamp;

  public Transaction() {
  }

  public Transaction(int payer, int payeeCount, int amount, int group) {
    this.payer = payer;
    this.payeeCount = payeeCount;
    this.amount = amount;
    this.group = group;
  }
}
