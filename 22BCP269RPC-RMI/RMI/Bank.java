import java.rmi.Remote;
import java.rmi.RemoteException;

public interface Bank extends Remote {
    int createAccount(String accountHolderName) throws RemoteException;
    double getBalance(int accountId) throws RemoteException;
    void deposit(int accountId, double amount) throws RemoteException;
    boolean withdraw(int accountId, double amount) throws RemoteException;
}