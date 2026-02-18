package net.nathcat.cost.handlers;

import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.Map;

import com.google.gson.Gson;
import com.sun.net.httpserver.HttpExchange;

import net.nathcat.cost.Server;
import net.nathcat.authcat.User;
import net.nathcat.cost.db.UserBalance;
import net.nathcat.cost.db.Utils;

/**
 * Endpoint handler which determins a user's current overall balance, and their
 * debts to each user of the group. See {@link Response} for response format.
 * 
 *
 */
public class DetermineBalance extends ApiHandler {
  public static class ResponseDebtRecord {
    public int user;
    public int balance;

    public ResponseDebtRecord(int u, int b) {
      this.user = u;
      this.balance = b;
    }
  }

  /**
   * Endpoint response format
   *
   */
  public static class Response {
    public int balance;
    public ResponseDebtRecord[] debts;
  }

  public DetermineBalance(Server server, String loggerName) {
    super(server, loggerName, new String[] { "GET" });
  }

  @Override
  public void handle(HttpExchange ex, User user, Map<String, String> getParams) throws IOException {
    int group;
    if (!getParams.containsKey("group")) {
      writeError(ex, 400);
      return;
    }
    try {
      group = Integer.parseInt(getParams.get("group"));
    } catch (NumberFormatException e) {
      writeError(ex, 400);
      return;
    }

    try {
      // If the user is a member of the group, get the balance and reply with
      // it
      if (Utils.isMemberOfGroup(server.db, user, group)) {
        UserBalance balance = Utils.determineBalance(server.db, group, user.id);
        ArrayList<ResponseDebtRecord> debts = new ArrayList<>();

        for (Integer member : balance.keySet()) {
          debts.add(new ResponseDebtRecord(member, balance.get(member)));
        }

        Response response = new Response();
        response.balance = balance.getBalance();
        response.debts = debts.toArray(new ResponseDebtRecord[0]);
        writeJson(ex, response);
      } else {
        writeError(ex, 403);
        return;
      }
    } catch (SQLException e) {
      writeError(ex, 500);
      e.printStackTrace();
      return;
    }
  }
}
