package net.nathcat.cost;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.sql.SQLException;

import com.google.gson.Gson;
import com.google.gson.JsonIOException;
import com.google.gson.JsonSyntaxException;

import net.nathcat.api.Server;
import net.nathcat.api.config.ServerConfig;
import net.nathcat.cost.handlers.DetermineBalance;
import net.nathcat.cost.handlers.EditTransaction;
import net.nathcat.cost.handlers.GetTransactions;
import net.nathcat.cost.handlers.LogTransaction;

public class App {
  public static void main(String[] args) throws JsonSyntaxException, JsonIOException, IOException, SQLException {
    Gson gson = new Gson();
    ServerConfig config = gson.fromJson(new InputStreamReader(new FileInputStream(new File(Server.SERVER_CONFIG_PATH))),
        ServerConfig.class);

    Server server = new Server(config);
    server.createContext("/api/determineBalance", new DetermineBalance(server, "/api/determineBalance"));
    server.createContext("/api/logTransaction", new LogTransaction(server, "/api/logTransaction"));
    server.createContext("/api/getTransactions", new GetTransactions(server, "/api/getTransactions"));
    server.createContext("/api/editTransaction", new EditTransaction(server, "/api/editTransaction"));

    server.start();
  }
}
