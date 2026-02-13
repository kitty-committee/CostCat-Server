package net.nathcat.cost.db;

import java.lang.reflect.Array;
import java.lang.reflect.InvocationTargetException;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.Arrays;

import net.nathcat.sql.DBType;
import net.nathcat.sql.Database;
import net.nathcat.sql.Query;

public class Utils {
  private static class Count implements DBType {
    public int count;
  }

  /**
   * Determine if a user is a member of a group or not
   * 
   * @param db    The database to query
   * @param user  The user to check
   * @param group The group to check membership in
   */
  public static boolean isMemberOfGroup(Database db, User user, int group) throws SQLException {
    return isMemberOfGroup(db, user.id, group);
  }

  /**
   * Determine if a user is a member of a group of not
   *
   * @param db    The database to query
   * @param user  The user to check
   * @param group The group to check membership in
   */
  public static boolean isMemberOfGroup(Database db, int user, int group) throws SQLException {
    // First check if the user is the owner of the group
    Query q;
    try {
      q = db.newQuery("SELECT count(*) AS 'count' FROM DataCat.`Groups` WHERE `owner` = ? AND id = ?");
      q
          .set(1, Integer.class, user)
          .set(2, Integer.class, group);
    } catch (NoSuchMethodException e) {
      throw new RuntimeException(e);
    }

    q.execute();
    Count count;
    try {
      count = net.nathcat.sql.Utils.extractResults(q.getResultSet(), Count.class)[0];
    } catch (InstantiationException | IllegalAccessException | NoSuchMethodException | InvocationTargetException
        | NoSuchFieldException | SQLException e) {
      throw new RuntimeException(e);
    }
    q.close();

    if (count.count != 1) {
      // If they are not the owner of the group, then check if they are a member
      try {
        q = db.newQuery("SELECT count(*) AS 'count' FROM DataCat.`Group_Members` WHERE `group` = ? AND `user` = ?");
        q
            .set(1, Integer.class, group)
            .set(2, Integer.class, user).execute();

        count = net.nathcat.sql.Utils.extractResults(q.getResultSet(), Count.class)[0];
        q.close();
      } catch (NoSuchMethodException | InstantiationException | IllegalAccessException | InvocationTargetException
          | NoSuchFieldException e) {
        throw new RuntimeException(e);
      }

      if (count.count != 1)
        return false;
    }

    return true;
  }

  /**
   * Get the list of transactions for a group
   *
   * @param db    The database to query
   * @param group The group to search for
   */
  public static Transaction[] getTransactions(Database db, int group) throws SQLException {
    Query q;
    Transaction[] t;
    try {
      q = db.newQuery("SELECT * FROM Transactions WHERE `group` = ? ORDER BY `timestamp` DESC");
      q
          .set(1, Integer.class, group)
          .execute();
      t = net.nathcat.sql.Utils.extractResults(q.getResultSet(), Transaction.class);
      q.close();
    } catch (NoSuchMethodException | InstantiationException | IllegalAccessException | InvocationTargetException
        | NoSuchFieldException e) {
      throw new RuntimeException(e);
    }

    return t;
  }

  /**
   * Log a new transaction
   *
   * @param db          The database to add to
   * @param transaction The transaction to add
   * @param payees      The list of payees
   */
  public static void logTransaction(Database db, Transaction transaction, int[] payees) throws SQLException {
    Query q;
    try {
      q = db.newQuery(
          "INSERT INTO Transactions (`group`, payer, amount, payeeCount, `timestamp`) VALUES (?, ?, ?, ?, unix_timestamp())");
      int id = q
          .set(1, Integer.class, transaction.group)
          .set(2, Integer.class, transaction.payer)
          .set(3, Integer.class, transaction.amount)
          .set(4, Integer.class, transaction.payeeCount)
          .set(5, Long.class, transaction.timestamp)
          .executeUpdate();

      q.close();

      for (int payee : payees) {
        q = db.newQuery("INSERT INTO Payees (`transaction`, `user`) VALUES (?, ?)");
        q
            .set(1, Integer.class, id)
            .set(2, Integer.class, payee)
            .execute();
        q.close();
      }
    } catch (NoSuchMethodException e) {
      throw new RuntimeException(e);
    }
  }

  public static User[] getGroupMembers(Database db, int group) throws SQLException {
    ArrayList<User> users = new ArrayList<>();

    Query q;
    try {
      q = db.newQuery(
          "SELECT SSO.Users.id as 'id', SSO.Users.username as 'username', SSO.Users.fullName as 'fullName', SSO.Users.pfpPath as 'pfpPath' AS 'count' FROM DataCat.`Groups` JOIN SSO.Users ON SSO.Users.id = `owner` WHERE id = ?");
      q
          .set(1, Integer.class, group)
          .execute();
    } catch (NoSuchMethodException e) {
      throw new RuntimeException(e);
    }

    try {
      User owner = net.nathcat.sql.Utils.extractResults(q.getResultSet(), User.class)[0];
      users.add(owner);
    } catch (InstantiationException | IllegalAccessException | NoSuchMethodException | InvocationTargetException
        | NoSuchFieldException | SQLException e) {
      throw new RuntimeException(e);
    }

    q.close();

    // If they are not the owner of the group, then check if they are a member
    try {
      q = db.newQuery(
          "SELECT SSO.Users.id as 'id', SSO.Users.username as 'username', SSO.Users.fullName as 'fullName', SSO.Users.pfpPath as 'pfpPath' FROM DataCat.`Group_Members` JOIN SSO.Users ON SSO.Users.id = `user` WHERE `group` = ?");
      q
          .set(1, Integer.class, group)
          .execute();

      User[] members = net.nathcat.sql.Utils.extractResults(q.getResultSet(), User.class);
      q.close();

      users.addAll(Arrays.asList(members));
    } catch (NoSuchMethodException | InstantiationException | IllegalAccessException | InvocationTargetException
        | NoSuchFieldException e) {
      throw new RuntimeException(e);
    }

    return users.toArray(new User[0]);
  }

  /**
   * Determine the balance of a group
   *
   * @param db    The database to query
   * @param group The group to calculate
   * @param user  The user ID of the user to calculate for
   */
  public static UserBalance determineBalance(Database db, int group, int user) throws SQLException {
    Transaction[] transactions;
    UserBalance balance = new UserBalance();
    Query q;

    User[] users = getGroupMembers(db, group);
    for (User member : users) {
      if (member.id == user)
        continue; // Skip the requesting user

      // Initialise this users balance
      balance.update(member, 0);

      // Calculate the amount this user has paid to us
      try {
        q = db.newQuery(
            "SELECT Transactions.* FROM Payees JOIN Transactions ON Payees.transaction = Transactions.id WHERE Transactions.payer = ? AND Payees.`user` = ? AND Transactions.`group` = ?");
        q
            .set(1, Integer.class, member.id)
            .set(2, Integer.class, user)
            .set(3, Integer.class, group)
            .execute();

        transactions = net.nathcat.sql.Utils.extractResults(q.getResultSet(), Transaction.class);
        q.close();
      } catch (NoSuchMethodException | InstantiationException | IllegalAccessException | InvocationTargetException
          | NoSuchFieldException e) {
        throw new RuntimeException(e);
      }

      for (Transaction t : transactions) {
        balance.update(member, -(t.amount / t.payeeCount));
      }

      // Calculate the amount we have paid to this user
      try {
        q = db.newQuery(
            "SELECT Transactions.* FROM Payees JOIN Transactions ON Payees.transaction = Transactions.id WHERE Transactions.payer = ? AND Payees.`user` = ? AND Transactions.`group` = ?");
        q
            .set(1, Integer.class, user)
            .set(2, Integer.class, member.id)
            .set(3, Integer.class, group)
            .execute();

        transactions = net.nathcat.sql.Utils.extractResults(q.getResultSet(), Transaction.class);
        q.close();
      } catch (NoSuchMethodException | InstantiationException | IllegalAccessException | InvocationTargetException
          | NoSuchFieldException e) {
        throw new RuntimeException(e);
      }

      for (Transaction t : transactions) {
        balance.update(member, t.amount / t.payeeCount);
      }
    }

    return balance;
  }

  public static Transaction getTransaction(Database db, int id) throws SQLException {
    Query q;
    Transaction t;

    try {
      q = db.newQuery("SELECT * FROM Transactions WHERE id = ?");
      q.set(1, Integer.class, id).execute();

      Transaction[] results = net.nathcat.sql.Utils.extractResults(q.getResultSet(), Transaction.class);
      if (results.length != 1)
        return null;
      else
        t = results[0];
    } catch (NoSuchMethodException | InstantiationException | IllegalAccessException | InvocationTargetException
        | NoSuchFieldException e) {
      throw new RuntimeException(e);
    }

    return t;
  }
}
