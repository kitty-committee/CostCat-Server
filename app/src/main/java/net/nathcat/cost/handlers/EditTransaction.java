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
import net.nathcat.sql.Query;

public class EditTransaction extends ApiHandler {
  public static class Request {
    public int amount;
    public int[] payees;
    public String description;
    public int id;
  }

  public EditTransaction(Server server, String loggerName) {
    super(server, loggerName);
  }

  @Override
  public void handle(HttpExchange ex, User user) throws IOException {
    InputStream in = ex.getRequestBody();
    Gson gson = new Gson();
    Request request = gson.fromJson(new InputStreamReader(in), Request.class);
    try {
      // Get the transaction
      Transaction t = Utils.getTransaction(server.db, request.id);

      if (t != null) {
        // Verify the requester owns the transaction
        if (t.payer == user.id) {
          try {
            Query q = server.db
                .newQuery("UPDATE Transactions SET amount = ?, payeeCount = ?, description = ? WHERE id = ?");
            q
                .set(1, Integer.class, request.amount)
                .set(2, Integer.class, request.payees.length)
                .set(3, String.class, request.description)
                .set(4, Integer.class, request.id)
                .executeUpdate();
            q.close();

            server.db.newQuery("DELETE FROM Payees WHERE `transaction` = ?")
                .set(1, Integer.class, request.id)
                .executeUpdate();

            for (int payee : request.payees) {
              server.db.newQuery("INSERT INTO Payees (`transaction`, `user`) VALUES (?, ?)")
                  .set(1, Integer.class, request.id)
                  .set(2, Integer.class, payee)
                  .executeUpdate();
            }
          } catch (NoSuchMethodException e) {
            throw new RuntimeException(e);
          }
        } else {
          writeError(ex, 403);
          return;
        }
      } else {
        writeJson(ex, new FailResponse("No transaction with the specified ID"));
        return;
      }
    } catch (SQLException e) {
      writeError(ex, 500);
      e.printStackTrace();
      return;
    }
  }
}
