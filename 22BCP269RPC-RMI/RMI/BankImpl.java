import java.rmi.server.UnicastRemoteObject;
import java.rmi.RemoteException;
import java.util.HashMap;
import java.util.Map;

public class BankImpl extends UnicastRemoteObject implements Bank {
    private static final long serialVersionUID = 1L;
    private Map<Integer, Account> accounts = new HashMap<>();
    private int accountCounter = 0;

    public BankImpl() throws RemoteException {
        super();
    }

    @Override
    public synchronized int createAccount(String accountHolderName) throws RemoteException {
        accountCounter++;
        accounts.put(accountCounter, new Account(accountHolderName, 0));
        return accountCounter;
    }

    @Override
    public synchronized double getBalance(int accountId) throws RemoteException {
        Account account = accounts.get(accountId);
        if (account == null) throw new RemoteException("Account not found");
        return account.getBalance();
    }

    @Override
    public synchronized void deposit(int accountId, double amount) throws RemoteException {
        Account account = accounts.get(accountId);
        if (account == null) throw new RemoteException("Account not found");
        account.deposit(amount);
    }

    @Override
    public synchronized boolean withdraw(int accountId, double amount) throws RemoteException {
        Account account = accounts.get(accountId);
        if (account == null) throw new RemoteException("Account not found");
        return account.withdraw(amount);
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
        if (amount > balance) return false;
        balance -= amount;
        return true;
    }
}
