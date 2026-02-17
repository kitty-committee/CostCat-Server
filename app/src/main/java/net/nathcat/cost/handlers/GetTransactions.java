package net.nathcat.cost.handlers;

import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.sql.SQLException;

import com.google.gson.Gson;
import com.sun.net.httpserver.HttpExchange;

import net.nathcat.cost.Server;
import net.nathcat.cost.db.Transaction;
import net.nathcat.authcat.User;
import net.nathcat.cost.db.Utils;

/**
 * Get the list of transactions for a group. The endpoint will reply with a JSON
 * array of {@link Transaction} objects. See {@link Request} for request format.
 *
 */
public class GetTransactions extends ApiHandler {
  public static class Request {
    public int group;
  }

  public GetTransactions(Server server, String loggerName) {
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
        Transaction[] transactions = Utils.getTransactions(server.db, request.group);
        writeJson(ex, transactions);
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
