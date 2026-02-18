package net.nathcat.cost.handlers;

import java.io.IOException;
import java.sql.SQLException;
import java.util.Arrays;
import java.util.Map;

import com.sun.net.httpserver.HttpExchange;

import net.nathcat.api.Server;
import net.nathcat.api.handlers.ApiHandler;
import net.nathcat.cost.db.Transaction;
import net.nathcat.authcat.User;
import net.nathcat.cost.db.Utils;

/**
 * Get the list of transactions for a group. The endpoint will reply with a JSON
 * array of {@link Transaction} objects.
 *
 */
public class GetTransactions extends ApiHandler {
  public GetTransactions(Server server, String loggerName) {
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
        Transaction[] transactions = Utils.getTransactions(server.db, group);
        for (Transaction t : transactions) {
          User[] payees = Utils.getTransactionPayees(server.db, t.id);
          t.payees = new int[payees.length];
          for (int i = 0; i < payees.length; i++) {
            t.payees[i] = payees[i].id;
          }
        }

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
