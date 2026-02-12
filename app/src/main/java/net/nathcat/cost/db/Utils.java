package net.nathcat.cost.db;

import java.lang.reflect.InvocationTargetException;
import java.sql.SQLException;

import net.nathcat.sql.DBType;
import net.nathcat.sql.Database;
import net.nathcat.sql.Query;

public class Utils {
  private class Count implements DBType {
    public int count;
  }

  /**
   * Determine if a user is a member of a group or not
   * 
   * @param db    The database to query
   * @param user  The user to check
   * @param group The group to check membership in
   */
  public boolean isMemberOfGroup(Database db, User user, int group) throws SQLException {
    return isMemberOfGroup(db, user.id, group);
  }

  /**
   * Determine if a user is a member of a group of not
   *
   * @param db    The database to query
   * @param user  The user to check
   * @param group The group to check membership in
   */
  public boolean isMemberOfGroup(Database db, int user, int group) throws SQLException {
    // First check if the user is the owner of the group
    Query q;
    try {
      q = db.newQuery("SELECT count(*) AS 'count' FROM DataCat.`Groups` WHERE `owner` = ? AND id = ?");
      q
          .set(0, int.class, user)
          .set(1, int.class, group);
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
            .set(0, int.class, group)
            .set(1, int.class, user).execute();

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
}
