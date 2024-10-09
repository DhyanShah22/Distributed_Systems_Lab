import java.rmi.server.UnicastRemoteObject;
import java.rmi.RemoteException;
import java.util.HashMap;
import java.util.Map;
import java.util.logging.FileHandler;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.logging.SimpleFormatter;

public class BankImpl extends UnicastRemoteObject implements Bank {
    private static final long serialVersionUID = 1L;
    private static final Logger logger = Logger.getLogger(BankImpl.class.getName());

    private Map<Integer, Account> accounts = new HashMap<>();
    private int accountCounter = 0;

    public BankImpl() throws RemoteException {
        super();
        
        // Set up file logging for BankImpl
        try {
            FileHandler fileHandler = new FileHandler("bank-impl.log", true); // Append to log file
            fileHandler.setFormatter(new SimpleFormatter());
            logger.addHandler(fileHandler);
            logger.setLevel(Level.INFO);
        } catch (Exception e) {
            logger.log(Level.SEVERE, "Failed to set up file logger", e);
        }
    }

    @Override
    public synchronized int createAccount(String accountHolderName) throws RemoteException {
        accountCounter++;
        Account newAccount = new Account(accountHolderName, 0);
        accounts.put(accountCounter, newAccount);
        
        // Log the account creation
        logger.info("Created account for " + accountHolderName + " with ID: " + accountCounter);
        return accountCounter;
    }

    @Override
    public synchronized double getBalance(int accountId) throws RemoteException {
        Account account = accounts.get(accountId);
        if (account == null) {
            logger.warning("Account not found with ID: " + accountId);
            throw new RemoteException("Account not found");
        }
        
        // Log balance retrieval
        logger.info("Fetched balance for account ID: " + accountId + " -> Balance: " + account.getBalance());
        return account.getBalance();
    }

    @Override
    public synchronized void deposit(int accountId, double amount) throws RemoteException {
        Account account = accounts.get(accountId);
        if (account == null) {
            logger.warning("Attempted deposit to non-existent account ID: " + accountId);
            throw new RemoteException("Account not found");
        }
        account.deposit(amount);
        
        // Log the deposit
        logger.info("Deposited " + amount + " to account ID: " + accountId);
    }

    @Override
    public synchronized boolean withdraw(int accountId, double amount) throws RemoteException {
        Account account = accounts.get(accountId);
        if (account == null) {
            logger.warning("Attempted withdrawal from non-existent account ID: " + accountId);
            throw new RemoteException("Account not found");
        }
        if (account.withdraw(amount)) {
            // Log successful withdrawal
            logger.info("Withdrew " + amount + " from account ID: " + accountId);
            return true;
        } else {
            // Log failed withdrawal due to insufficient funds
            logger.warning("Insufficient funds for withdrawal from account ID: " + accountId);
            return false;
        }
    }
}

class Account {
    private String holderName;
    private double balance;

    public Account(String holderName, double balance) {
        this.holderName = holderName;
        this.balance = balance;
    }

    public synchronized double getBalance() {
        return balance;
    }

    public synchronized void deposit(double amount) {
        balance += amount;
    }

    public synchronized boolean withdraw(double amount) {
        if (amount > balance) {
            return false;
        }
        balance -= amount;
        return true;
    }
}
