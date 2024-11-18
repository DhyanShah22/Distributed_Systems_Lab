import java.rmi.Naming;
import java.util.Scanner;

public class BankClient {
    public static void main(String[] args) {
        try {
            Bank bank = (Bank) Naming.lookup("//localhost:1100/BankService");
            Scanner scanner = new Scanner(System.in);

            while (true) {
                System.out.println("\n1. Create Account");
                System.out.println("2. Deposit");
                System.out.println("3. Withdraw");
                System.out.println("4. Check Balance");
                System.out.println("5. Exit");

                System.out.print("Enter your choice: ");
                int choice = scanner.nextInt();

                switch (choice) {
                    case 1:
                        System.out.print("Enter the account holder name: ");
                        scanner.nextLine();
                        String name = scanner.nextLine();
                        int accountId = bank.createAccount(name);
                        System.out.println("Account created with ID: " + accountId);
                        break;
                    case 2:
                        System.out.print("Enter account ID: ");
                        int depositId = scanner.nextInt();
                        System.out.print("Enter amount to deposit: ");
                        double depositAmount = scanner.nextDouble();
                        bank.deposit(depositId, depositAmount);
                        System.out.println("Amount deposited.");
                        break;
                    case 3:
                        System.out.print("Enter account ID: ");
                        int withdrawId = scanner.nextInt();
                        System.out.print("Enter amount to withdraw: ");
                        double withdrawAmount = scanner.nextDouble();
                        if (bank.withdraw(withdrawId, withdrawAmount)) {
                            System.out.println("Withdrawal successful.");
                        } else {
                            System.out.println("Insufficient funds.");
                        }
                        break;
                    case 4:
                        System.out.print("Enter account ID: ");
                        int balanceId = scanner.nextInt();
                        System.out.println("Account balance: " + bank.getBalance(balanceId));
                        break;
                    case 5:
                        System.out.println("Exiting...");
                        scanner.close();
                        return;
                    default:
                        System.out.println("Invalid choice. Try again.");
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
