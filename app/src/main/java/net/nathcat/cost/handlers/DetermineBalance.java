package net.nathcat.cost.handlers;

import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.sql.SQLException;
import java.util.ArrayList;

import com.google.gson.Gson;
import com.sun.net.httpserver.HttpExchange;

import net.nathcat.cost.Server;
import net.nathcat.cost.db.User;
import net.nathcat.cost.db.UserBalance;
import net.nathcat.cost.db.Utils;

/**
 * Endpoint handler which determins a user's current overall balance, and their
 * debts to each user of the group. See {@link Response} for response format.
 * See {@link Request} for the request format.
 *
 */
public class DetermineBalance extends ApiHandler {
  public static class Request {
    public int group;
  }

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
    super(server, loggerName);
  }

  @Override
  public void handle(HttpExchange ex, User user) throws IOException {
    InputStream in = ex.getRequestBody();
    Gson gson = new Gson();
    Request request = gson.fromJson(new InputStreamReader(in), Request.class);
    try {
      // If the user is a member of the group, get the balance and reply with
      // it
      if (Utils.isMemberOfGroup(server.db, user, request.group)) {
        UserBalance balance = Utils.determineBalance(server.db, request.group, user.id);
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
