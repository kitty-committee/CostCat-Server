package net.nathcat.cost.handlers;

import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.sql.SQLException;

import com.google.gson.Gson;
import com.sun.net.httpserver.HttpExchange;

import net.nathcat.cost.Server;
import net.nathcat.cost.db.Transaction;
import net.nathcat.cost.db.User;
import net.nathcat.cost.db.Utils;

/**
 * Log a new transaction via this endpoint. See {@link Request} for the request
 * format. The endpoint will reply with a {@link SuccessResponse} upon success.
 *
 */
public class LogTransaction extends ApiHandler {
  public static class Request {
    public int group;
    public int amount;
    public int[] payees;
  }

  public LogTransaction(Server server, String loggerName) {
    super(server, loggerName);
  }

  @Override
  public void handle(HttpExchange ex, User user) throws IOException {
    InputStream in = ex.getRequestBody();
    Gson gson = new Gson();
    Request request = gson.fromJson(new InputStreamReader(in), Request.class);
    try {
      // If the user is a member of the group, log the transaction and reply with
      // success
      if (Utils.isMemberOfGroup(server.db, user, request.group)) {
        Transaction t = new Transaction(user.id, request.payees.length, request.amount, request.group);
        Utils.logTransaction(server.db, t, request.payees);
        writeJson(ex, new SuccessResponse());

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
