import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;

public class BankServer {
    public static void main(String[] args) {
        try {
            Bank bank = new BankImpl();
            Registry registry = LocateRegistry.createRegistry(1100);
            registry.bind("BankService", bank);
            System.out.println("Bank server started on port 1100");
            } catch (Exception e) {
                e.printStackTrace();
        }
    }
}
