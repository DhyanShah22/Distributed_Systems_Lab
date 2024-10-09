import java.rmi.Naming;
import java.util.InputMismatchException;
import java.util.Scanner;

public class BankClient {
    public static void main(String[] args) {
        try {
            Bank bank = (Bank) Naming.lookup("//localhost:1100/BankService");
            Scanner scanner = new Scanner(System.in);
            boolean running = true;

            while (running) {
                System.out.println("\n1. Create Account");
                System.out.println("2. Deposit");
                System.out.println("3. Withdraw");
                System.out.println("4. Check Balance");
                System.out.println("5. Exit");

                try {
                    System.out.print("Enter your choice: ");
                    int choice = scanner.nextInt();  // This is where the exception could happen
                    scanner.nextLine();  // To clear the buffer after nextInt()

                    switch (choice) {
                        case 1:
                            System.out.print("Enter account holder name: ");
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
                            boolean success = bank.withdraw(withdrawId, withdrawAmount);
                            if (success) {
                                System.out.println("Withdrawal successful.");
                            } else {
                                System.out.println("Insufficient funds.");
                            }
                            break;
                        case 4:
                            System.out.print("Enter account ID: ");
                            int balanceId = scanner.nextInt();
                            double balance = bank.getBalance(balanceId);
                            System.out.println("Account balance: " + balance);
                            break;
                        case 5:
                            System.out.println("Exiting...");
                            running = false;
                            break;
                        default:
                            System.out.println("Invalid choice. Please try again.");
                            break;
                    }
                } catch (InputMismatchException e) {
                    System.out.println("Invalid input. Please enter a valid number.");
                    scanner.nextLine();  // Clear the scanner buffer
                }
            }
            scanner.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
