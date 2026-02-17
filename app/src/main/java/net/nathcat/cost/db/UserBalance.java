package net.nathcat.cost.db;

import java.util.HashMap;

import net.nathcat.authcat.User;

/**
 *
 * Represents the balance of a group.
 *
 */
public class UserBalance extends HashMap<Integer, Integer> {
  public int getBalance() {
    int total = 0;
    for (Integer key : keySet()) {
      total += get(key);
    }

    return total;
  }

  /**
   * Get the balance of a user
   *
   */
  public Integer get(User u) {
    return get(u.id);
  }

  /**
   * Get the balance of a user
   *
   * @param u The user's id
   */
  @Override
  public Integer get(Object u) {
    Integer v = super.get(u);
    if (v == null)
      return 0;
    else
      return v;
  }

  public void update(User u, int change) {
    put(u.id, get(u) + change);
  }
}
