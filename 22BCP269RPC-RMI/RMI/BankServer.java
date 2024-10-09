import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.util.logging.FileHandler;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.logging.SimpleFormatter;

public class BankServer {
    private static final Logger logger = Logger.getLogger(BankServer.class.getName());

    public static void main(String[] args) {
        try {
            // Set up file logging
            FileHandler fileHandler = new FileHandler("bank-server.log", true); // Append to log file
            fileHandler.setFormatter(new SimpleFormatter());
            logger.addHandler(fileHandler);
            logger.setLevel(Level.INFO);

            // Start RMI registry
            BankImpl bank = new BankImpl();
            Registry registry = LocateRegistry.createRegistry(1100);
            registry.bind("BankService", bank);

            // Log that the server is running
            System.out.println("Bank server is running...");
            logger.info("Bank server started and running on port 1100");

        } catch (Exception e) {
            // Log the exception
            logger.log(Level.SEVERE, "Server exception occurred", e);
            e.printStackTrace();
        }
    }
}
