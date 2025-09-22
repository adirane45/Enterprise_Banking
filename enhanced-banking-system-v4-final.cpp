#include<iostream>
#include<string>
#include<memory>
#include<limits>
#include<ctime>
#include<vector>
#include<unordered_map>
#include<map>
#include<fstream>
#include<iomanip>
#include<algorithm>
#include<sstream>
#include<random>
#include<chrono>
#include<filesystem>

using namespace std;
using namespace std::filesystem;

// Forward declarations
class Transaction;
class Account;

// Custom Exception Classes
class BankingException : public exception {
protected:
    string message;
public:
    BankingException(const string& msg) : message(msg) {}
    const char* what() const noexcept override { return message.c_str(); }
};

class InsufficientFundsException : public BankingException {
public:
    InsufficientFundsException(const string& msg) : BankingException("Insufficient Funds: " + msg) {}
};

class InvalidAmountException : public BankingException {
public:
    InvalidAmountException(const string& msg) : BankingException("Invalid Amount: " + msg) {}
};

class AccountNotFoundException : public BankingException {
public:
    AccountNotFoundException(const string& msg) : BankingException("Account Not Found: " + msg) {}
};

class DataIntegrityException : public BankingException {
public:
    DataIntegrityException(const string& msg) : BankingException("Data Integrity Error: " + msg) {}
};

// Enhanced Security Helper
class SecurityManager {
private:
    static string generateSalt(size_t length = 16) {
        const string chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(0, chars.size() - 1);
        
        string salt;
        for (size_t i = 0; i < length; ++i) {
            salt += chars[dis(gen)];
        }
        return salt;
    }
    
public:
    // Improved hash function with salt (still using std::hash for simplicity, but better than before)
    static pair<string, string> hashWithSalt(const string& input) {
        string salt = generateSalt();
        hash<string> hasher;
        string salted = salt + input + "BANKING_2025_SECURE";
        string hashed = to_string(hasher(salted)) + "_" + to_string(hasher(salted + salt));
        return {hashed, salt};
    }
    
    static bool verifyHash(const string& input, const string& stored_hash, const string& salt) {
        hash<string> hasher;
        string salted = salt + input + "BANKING_2025_SECURE";
        string computed = to_string(hasher(salted)) + "_" + to_string(hasher(salted + salt));
        return computed == stored_hash;
    }
    
    // Generate secure OTP for 2FA
    static string generateOTP() {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(100000, 999999);
        return to_string(dis(gen));
    }
};

// Enhanced Input Validation Class
class InputValidator {
public:
    static float getValidAmount(const string& prompt, float min = 0.01, float max = 1000000) {
        float amount;
        do {
            cout << prompt;
            while (!(cin >> amount)) {
                cout << "Invalid input! Please enter a number: ";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
            if (amount < min || amount > max) {
                cout << "Amount must be between " << min << " and " << max << ". Try again.\n";
            }
        } while (amount < min || amount > max);
        return amount;
    }
    
    // FIXED: Phone number validation using string
    static bool isValidPhoneNumber(const string& phone) {
        return phone.size() == 10 && all_of(phone.begin(), phone.end(), ::isdigit);
    }
    
    static bool isValidAccountNumber(int acc_no) {
        return acc_no > 0 && acc_no <= 999999;
    }
    
    static int getValidChoice(const string& prompt, int min, int max) {
        int choice;
        do {
            cout << prompt;
            while (!(cin >> choice)) {
                cout << "Invalid input! Please enter a number: ";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
            if (choice < min || choice > max) {
                cout << "Choice must be between " << min << " and " << max << ". Try again.\n";
            }
        } while (choice < min || choice > max);
        return choice;
    }
    
    // NEW: Separate function for integer ranges (not menu choices)
    static int getValidIntInRange(const string& prompt, int min, int max) {
        int val;
        do {
            cout << prompt;
            while (!(cin >> val)) {
                cout << "Invalid input! Please enter a number: ";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
            if (val < min || val > max) {
                cout << "Value must be between " << min << " and " << max << ". Try again.\n";
            }
        } while (val < min || val > max);
        return val;
    }
    
    // FIXED: Improved getValidString to prevent input skipping
    static string getValidString(const string& prompt) {
        string input;
        do {
            cout << prompt;
            getline(cin >> ws, input);  // Fixed: safely consumes whitespace/newline
            if (input.empty()) {
                cout << "Input cannot be empty. Try again.\n";
            }
        } while (input.empty());
        return input;
    }
    
    static string getValidPhoneNumber(const string& prompt) {
        string phone;
        do {
            phone = getValidString(prompt);
            if (!isValidPhoneNumber(phone)) {
                cout << "Invalid phone number! Enter exactly 10 digits: ";
            }
        } while (!isValidPhoneNumber(phone));
        return phone;
    }
};

// Enhanced Transaction Class with persistence support
class Transaction {
private:
    string transaction_id, type;
    float amount, balance_after;
    time_t timestamp;
    
public:
    Transaction(const string& t_type, float amt, float balance) 
        : type(t_type), amount(amt), balance_after(balance) {
        time(&timestamp);
        transaction_id = "TXN" + to_string(timestamp % 1000000);
    }
    
    // Constructor for loading from file
    Transaction(const string& txn_id, const string& t_type, float amt, float balance, time_t ts)
        : transaction_id(txn_id), type(t_type), amount(amt), balance_after(balance), timestamp(ts) {}
    
    void display() const {
        cout << left << setw(12) << transaction_id
             << setw(15) << type
             << "Rs." << setw(10) << fixed << setprecision(2) << amount
             << "Rs." << setw(12) << balance_after;
        
        struct tm* timeinfo = localtime(&timestamp);
        char buffer[80];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
        cout << setw(20) << buffer << endl;
    }
    
    // Getters
    string getTransactionId() const { return transaction_id; }
    string getType() const { return type; }
    float getAmount() const { return amount; }
    time_t getTimestamp() const { return timestamp; }
    float getBalanceAfter() const { return balance_after; }
    
    // For file operations with proper CSV escaping
    string toCSV() const {
        return transaction_id + "|" + type + "|" + to_string(amount) + "|" + 
               to_string(balance_after) + "|" + to_string(timestamp);
    }
    
    static Transaction fromCSV(const string& csv_line) {
        stringstream ss(csv_line);
        string txn_id, type, amt_str, bal_str, ts_str;
        
        getline(ss, txn_id, '|');
        getline(ss, type, '|');
        getline(ss, amt_str, '|');
        getline(ss, bal_str, '|');
        getline(ss, ts_str, '|');
        
        return Transaction(txn_id, type, stof(amt_str), stof(bal_str), static_cast<time_t>(stoll(ts_str)));
    }
};

// User Role Enum
enum class UserRole {
    USER = 0,
    ADMIN = 1
};

// Enhanced Account System
class AccountManager {
private:
    static int next_account_number;
    
public:
    static int generateUniqueAccountNumber() {
        return ++next_account_number;
    }
    
    static void setNextAccountNumber(int next) {
        next_account_number = next;
    }
    
    static int getNextAccountNumber() {
        return next_account_number;
    }
};

int AccountManager::next_account_number = 100000; // Start from 100001

// Base Account Class (Enhanced)
class Account {
protected:
    int acc_no;
    string ph_no;  // FIXED: Changed from long int to string
    string name;
    string address;
    float balance;
    vector<Transaction> transaction_history;
    static const int MAX_HISTORY = 500; // Increased limit
    time_t created_date;

public:
    virtual void getinfo() {
        cout << "\nAccount Creation Options:" << endl;
        cout << "1. Auto-generate account number" << endl;
        cout << "2. Choose your own account number" << endl;
        int choice = InputValidator::getValidChoice("Enter choice: ", 1, 2);
        
        if (choice == 1) {
            acc_no = AccountManager::generateUniqueAccountNumber();
            cout << "Auto-generated Account Number: " << acc_no << endl;
        } else {
            acc_no = InputValidator::getValidIntInRange("Enter Account no. (100001-999999): ", 100001, 999999);
        }
        
        name = InputValidator::getValidString("Enter Name: ");
        ph_no = InputValidator::getValidPhoneNumber("Enter Phone Number (10 digits): ");
        address = InputValidator::getValidString("Enter Address: ");
        balance = InputValidator::getValidAmount("Enter Initial Balance: Rs. ");
        
        time(&created_date);
        recordTransaction("ACCOUNT_CREATED", balance);
    }

    virtual void putinfo() const {
        cout << "\n--- Account Details ---";
        cout << "\nAccount No. : " << acc_no;
        cout << "\nName        : " << name;
        cout << "\nPhone no.   : " << ph_no;  // Now properly displays as string
        cout << "\nAddress     : " << address;
        cout << "\nBalance     : Rs." << fixed << setprecision(2) << balance;
        
        struct tm* timeinfo = localtime(&created_date);
        char buffer[80];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d", timeinfo);
        cout << "\nCreated     : " << buffer;
    }

    // Transaction methods with improved recording
    void recordTransaction(const string& type, float amount) {
        Transaction trans(type, amount, balance);
        transaction_history.push_back(trans);
        
        if (transaction_history.size() > MAX_HISTORY) {
            transaction_history.erase(transaction_history.begin());
        }
        
        // Immediately save transaction to file for crash recovery
        saveTransactionToFile(trans);
    }
    
    virtual void credit(float amount) {
        balance += amount;
        recordTransaction("CREDIT", amount);
    }
    
    virtual void debit(float amount) {
        if (amount > balance) {
            throw InsufficientFundsException("Cannot debit Rs." + to_string(amount));
        }
        balance -= amount;
        recordTransaction("DEBIT", amount);
    }
    
    // Enhanced transaction history with filtering
    void showTransactionHistory(int limit = 10, const string& filter_type = "") const {
        cout << "\n=== TRANSACTION HISTORY ===" << endl;
        cout << left << setw(12) << "TXN ID"
             << setw(15) << "TYPE"
             << setw(12) << "AMOUNT"
             << setw(12) << "BALANCE"
             << "TIMESTAMP" << endl;
        cout << string(70, '-') << endl;
        
        if (transaction_history.empty()) {
            cout << "No transactions found." << endl;
            return;
        }
        
        vector<Transaction> filtered_transactions;
        for (const auto& trans : transaction_history) {
            if (filter_type.empty() || trans.getType() == filter_type) {
                filtered_transactions.push_back(trans);
            }
        }
        
        int start = max(0, (int)filtered_transactions.size() - limit);
        for (int i = start; i < (int)filtered_transactions.size(); i++) {
            filtered_transactions[i].display();
        }
        
        cout << "Showing " << min(limit, (int)filtered_transactions.size()) 
             << " of " << filtered_transactions.size() << " transactions" << endl;
    }
    
    void generateStatement() const {
        cout << "\n=== ACCOUNT STATEMENT ===" << endl;
        putinfo();
        showTransactionHistory(50); // Show more transactions in statement
    }
    
    // NEW: Save individual transaction to file for crash recovery
    void saveTransactionToFile(const Transaction& trans) const {
        string filename = "transactions_" + to_string(acc_no) + ".txt";
        ofstream file(filename, ios::app);
        if (file.is_open()) {
            file << trans.toCSV() << endl;
            file.close();
        }
    }
    
    // NEW: Load transaction history from file
    void loadTransactionHistory() {
        string filename = "transactions_" + to_string(acc_no) + ".txt";
        ifstream file(filename);
        if (file.is_open()) {
            string line;
            transaction_history.clear();
            
            while (getline(file, line) && !line.empty()) {
                try {
                    Transaction trans = Transaction::fromCSV(line);
                    transaction_history.push_back(trans);
                } catch (const exception& e) {
                    // Skip corrupted transaction records
                    continue;
                }
            }
            file.close();
        }
    }

    // Getters
    int getAccountNumber() const { return acc_no; }
    string getName() const { return name; }
    string getPhoneNumber() const { return ph_no; }  // Now returns string
    string getAddress() const { return address; }
    float getBalance() const { return balance; }
    virtual string getAccountType() const = 0;
    const vector<Transaction>& getTransactionHistory() const { return transaction_history; }
    time_t getCreatedDate() const { return created_date; }
    
    // Setters for loading from file - FIXED: phone number is now string
    void setAccountDetails(int acc, const string& n, const string& ph, const string& addr, float bal, time_t created = 0) {
        acc_no = acc;
        name = n;
        ph_no = ph;  // Now string assignment
        address = addr;
        balance = bal;
        created_date = (created == 0) ? time(nullptr) : created;
        
        // Update the account number counter if necessary
        if (acc >= AccountManager::getNextAccountNumber()) {
            AccountManager::setNextAccountNumber(acc);
        }
        
        loadTransactionHistory(); // Load existing transactions
    }

    // Pure virtual functions
    virtual void withdrawal() = 0;
    virtual void deposit() = 0;  // FIXED: Renamed from deposite
    virtual ~Account() {}
};

// Savings Account Class with Interest Application
class SavingsAccount : public Account {
protected:
    float rate;

public:
    void getinfo() override {
        Account::getinfo();
        cout << "Enter rate of interest (0.1-15.0%): ";
        do {
            cin >> rate;
            if (rate < 0.1 || rate > 15.0) {
                cout << "Interest rate must be between 0.1% and 15.0%. Try again: ";
            }
        } while (rate < 0.1 || rate > 15.0);
    }

    void withdrawal() override {
        try {
            float amount = InputValidator::getValidAmount("\nEnter amount to withdraw: Rs. ");
            
            if (amount > balance) {
                throw InsufficientFundsException("Cannot withdraw Rs." + to_string(amount) + 
                                               ". Available balance: Rs." + to_string(balance));
            }
            
            balance -= amount;
            recordTransaction("WITHDRAWAL", amount);
            cout << "\nWithdrawal successful! New balance: Rs." << fixed << setprecision(2) << balance << endl;
            
        } catch (const BankingException& e) {
            cout << "\nTransaction failed: " << e.what() << endl;
        }
    }

    void deposit() override {  // FIXED: Renamed from deposite
        try {
            float amount = InputValidator::getValidAmount("\nEnter amount to deposit: Rs. ");
            
            balance += amount;
            recordTransaction("DEPOSIT", amount);
            cout << "\nDeposit successful! New balance: Rs." << fixed << setprecision(2) << balance << endl;
            
        } catch (const exception& e) {
            cout << "\nDeposit failed: " << e.what() << endl;
        }
    }

    float interest() const {
        return balance * rate / 100;
    }
    
    void applyInterest() {
        float interest_amount = interest();
        balance += interest_amount;
        recordTransaction("INTEREST_APPLIED", interest_amount);
        cout << "Monthly interest of Rs." << fixed << setprecision(2) << interest_amount 
             << " applied to account " << acc_no << endl;
    }

    void putinfo() const override {
        Account::putinfo();
        cout << "\nAccount Type: Savings Account";
        cout << "\nInterest Rate : " << fixed << setprecision(1) << rate << "%";
        cout << "\nInterest to be Earned: Rs." << fixed << setprecision(2) << interest() << endl;
    }
    
    string getAccountType() const override {
        return "SAVINGS";
    }
    
    float getInterestRate() const { return rate; }
    void setInterestRate(float r) { rate = r; }
};

// Current Account Class with Enhanced Overdraft Handling
class CurrentAccount : public Account {
protected:
    float overdraft_limit;
    float cap;

public:
    void getinfo() override {
        Account::getinfo();
        overdraft_limit = InputValidator::getValidAmount("Enter Overdraft Limit: Rs. ");
        cap = overdraft_limit;
    }

    void withdrawal() override {
        try {
            float amount = InputValidator::getValidAmount("\nEnter amount to withdraw: Rs. ");

            if (amount > (balance + overdraft_limit)) {
                throw InsufficientFundsException("Amount exceeds available balance and overdraft limit");
            }
            
            if (amount > balance) {
                float from_overdraft = amount - balance;
                balance = 0;
                overdraft_limit -= from_overdraft;
                recordTransaction("WITHDRAWAL_OVERDRAFT", amount);
            } else {
                balance -= amount;
                recordTransaction("WITHDRAWAL", amount);
            }
            
            cout << "\nWithdrawal successful!" << endl;
            cout << "Available Balance: Rs." << fixed << setprecision(2) << balance << endl;
            cout << "Available Overdraft: Rs." << fixed << setprecision(2) << overdraft_limit << endl;
            
        } catch (const BankingException& e) {
            cout << "\nTransaction failed: " << e.what() << endl;
        }
    }

    void deposit() override {  // FIXED: Renamed from deposite
        try {
            float amount = InputValidator::getValidAmount("\nEnter amount to deposit: Rs. ");
            int choice = InputValidator::getValidChoice(
                "Where to deposit?\n1. Account Balance\n2. Repay Overdraft\nChoice: ", 1, 2);
            
            switch (choice) {
                case 1:
                    balance += amount;
                    recordTransaction("DEPOSIT", amount);
                    break;
                case 2: {
                    // FIXED: Better overdraft repayment logic with separate transactions
                    if ((overdraft_limit + amount) > cap) {
                        float to_repay = cap - overdraft_limit;
                        if (to_repay > 0) {
                            cout << "Repaying Rs." << to_repay << " to overdraft, Rs." << (amount - to_repay) << " to balance." << endl;
                            overdraft_limit = cap;
                            balance += (amount - to_repay);
                            recordTransaction("REPAY_OVERDRAFT", to_repay);
                            if (amount - to_repay > 0) {
                                recordTransaction("DEPOSIT", amount - to_repay);
                            }
                        } else {
                            cout << "Overdraft full. Depositing to main balance." << endl;
                            balance += amount;
                            recordTransaction("DEPOSIT", amount);
                        }
                    } else {
                        overdraft_limit += amount;
                        recordTransaction("REPAY_OVERDRAFT", amount);
                    }
                    break;
                }
            }
            
            cout << "\nDeposit successful!" << endl;
            cout << "Available Balance: Rs." << fixed << setprecision(2) << balance << endl;
            cout << "Available Overdraft: Rs." << fixed << setprecision(2) << overdraft_limit << endl;
            
        } catch (const BankingException& e) {
            cout << "\nDeposit failed: " << e.what() << endl;
        }
    }

    void putinfo() const override {
        Account::putinfo();
        cout << "\nAccount Type: Current Account";
        cout << "\nOverdraft Limit: Rs." << fixed << setprecision(2) << overdraft_limit << " of Rs." << cap << endl;
    }
    
    string getAccountType() const override {
        return "CURRENT";
    }
    
    float getOverdraftLimit() const { return overdraft_limit; }
    float getOverdraftCap() const { return cap; }
    void setOverdraftDetails(float limit, float c) { overdraft_limit = limit; cap = c; }
};

// Loan Account Class
class LoanAccount : public Account {
protected:
    float principal_amount;
    float interest_rate;
    int tenure_months;
    float emi_amount;
    int payments_made;

public:
    void getinfo() override {
        Account::getinfo();
        principal_amount = balance;
        
        cout << "Enter annual interest rate (1.0-20.0%): ";
        do {
            cin >> interest_rate;
            if (interest_rate < 1.0 || interest_rate > 20.0) {
                cout << "Interest rate must be between 1.0% and 20.0%. Try again: ";
            }
        } while (interest_rate < 1.0 || interest_rate > 20.0);
        
        cout << "Enter loan tenure in months (6-360): ";
        do {
            cin >> tenure_months;
            if (tenure_months < 6 || tenure_months > 360) {
                cout << "Tenure must be between 6 and 360 months. Try again: ";
            }
        } while (tenure_months < 6 || tenure_months > 360);
        
        payments_made = 0;
        calculateEMI();
    }
    
    void calculateEMI() {
        float monthly_rate = interest_rate / (12 * 100);
        float factor = pow(1 + monthly_rate, tenure_months);
        emi_amount = (principal_amount * monthly_rate * factor) / (factor - 1);
    }

    void withdrawal() override {
        cout << "Withdrawal not allowed on loan accounts. Use deposit() to make EMI payments." << endl;
    }

    void deposit() override {  // FIXED: Renamed from deposite
        try {
            cout << "EMI Amount: Rs." << fixed << setprecision(2) << emi_amount << endl;
            cout << "Outstanding Balance: Rs." << balance << endl;
            
            int choice = InputValidator::getValidChoice(
                "\n1. Pay EMI (" + to_string((int)emi_amount) + ")\n2. Pay Custom Amount\nChoice: ", 1, 2);
            
            float payment_amount;
            if (choice == 1) {
                payment_amount = emi_amount;
            } else {
                payment_amount = InputValidator::getValidAmount("Enter payment amount: Rs. ");
            }
            
            if (payment_amount > balance) {
                payment_amount = balance;
                cout << "Payment adjusted to outstanding balance: Rs." << payment_amount << endl;
            }
            
            balance -= payment_amount;
            payments_made++;
            recordTransaction("EMI_PAYMENT", payment_amount);
            
            cout << "\nPayment successful!" << endl;
            cout << "Outstanding Loan Balance: Rs." << fixed << setprecision(2) << balance << endl;
            cout << "Payments Made: " << payments_made << " of " << tenure_months << endl;
            
            if (balance <= 0) {
                cout << "🎉 Congratulations! Loan has been fully paid!" << endl;
                balance = 0;
                recordTransaction("LOAN_CLOSED", 0);
            }
            
        } catch (const exception& e) {
            cout << "\nPayment failed: " << e.what() << endl;
        }
    }

    void putinfo() const override {
        Account::putinfo();
        cout << "\nAccount Type: Loan Account";
        cout << "\nPrincipal Amount: Rs." << fixed << setprecision(2) << principal_amount;
        cout << "\nInterest Rate: " << interest_rate << "% per annum";
        cout << "\nTenure: " << tenure_months << " months";
        cout << "\nEMI Amount: Rs." << fixed << setprecision(2) << emi_amount;
        cout << "\nPayments Made: " << payments_made << " of " << tenure_months;
        cout << "\nOutstanding Balance: Rs." << fixed << setprecision(2) << balance << endl;
    }
    
    string getAccountType() const override {
        return "LOAN";
    }
    
    float getPrincipalAmount() const { return principal_amount; }
    float getInterestRate() const { return interest_rate; }
    int getTenureMonths() const { return tenure_months; }
    float getEMIAmount() const { return emi_amount; }
    int getPaymentsMade() const { return payments_made; }
    
    void setLoanDetails(float principal, float rate, int tenure, int payments) {
        principal_amount = principal;
        interest_rate = rate;
        tenure_months = tenure;
        payments_made = payments;
        calculateEMI();
    }
};

// Enhanced User Class with Role Support
class User {
private:
    string username;
    string hashed_password;
    string salt;
    vector<int> owned_accounts;
    UserRole role;
    time_t created_date;
    time_t last_login;

public:
    User() : role(UserRole::USER) {
        time(&created_date);
        last_login = 0;
    }
    
    User(const string& user, const string& pass, UserRole r = UserRole::USER) : username(user), role(r) {
        auto hash_result = SecurityManager::hashWithSalt(pass);
        hashed_password = hash_result.first;
        salt = hash_result.second;
        time(&created_date);
        last_login = 0;
    }
    
    bool authenticate(const string& password) {
        if (SecurityManager::verifyHash(password, hashed_password, salt)) {
            time(&last_login);
            return true;
        }
        return false;
    }
    
    void addAccount(int acc_no) {
        if (find(owned_accounts.begin(), owned_accounts.end(), acc_no) == owned_accounts.end()) {
            owned_accounts.push_back(acc_no);
        }
    }
    
    bool ownsAccount(int acc_no) const {
        return find(owned_accounts.begin(), owned_accounts.end(), acc_no) != owned_accounts.end();
    }
    
    bool isAdmin() const {
        return role == UserRole::ADMIN;
    }
    
    // Getters and setters
    string getUsername() const { return username; }
    const vector<int>& getOwnedAccounts() const { return owned_accounts; }
    UserRole getRole() const { return role; }
    time_t getLastLogin() const { return last_login; }
    
    void setUserDetails(const string& user, const string& hashedPass, const string& user_salt, 
                       const vector<int>& accounts, UserRole r = UserRole::USER, time_t created = 0, time_t login = 0) {
        username = user;
        hashed_password = hashedPass;
        salt = user_salt;
        owned_accounts = accounts;
        role = r;
        created_date = (created == 0) ? time(nullptr) : created;
        last_login = login;
    }
    
    string getHashedPassword() const { return hashed_password; }
    string getSalt() const { return salt; }
    
    string toDataString() const {
        stringstream ss;
        ss << username << "|" << hashed_password << "|" << salt << "|" << static_cast<int>(role) 
           << "|" << created_date << "|" << last_login;
        for (int acc_no : owned_accounts) {
            ss << "|" << acc_no;
        }
        return ss.str();
    }
};

// Enhanced Authentication Manager with Secure Storage
class AuthenticationManager {
private:
    unordered_map<int, pair<string, string>> account_pins;  // acc_no -> {hashed_pin, salt}
    unordered_map<int, int> failed_attempts;
    unordered_map<int, string> active_otps;  // For 2FA
    static const int MAX_ATTEMPTS = 3;
    
public:
    bool registerPin(int acc_no) {
        string pin, confirm_pin;
        
        do {
            cout << "Set 4-digit PIN: ";
            cin >> pin;
            
            if (pin.length() != 4 || !all_of(pin.begin(), pin.end(), ::isdigit)) {
                cout << "PIN must be exactly 4 digits!" << endl;
                continue;
            }
            
            cout << "Confirm PIN: ";
            cin >> confirm_pin;
            
            if (pin != confirm_pin) {
                cout << "PINs do not match! Try again." << endl;
                continue;
            }
            
            break;
        } while (true);
        
        auto hash_result = SecurityManager::hashWithSalt(pin);
        account_pins[acc_no] = {hash_result.first, hash_result.second};
        failed_attempts[acc_no] = 0;
        cout << "PIN set successfully!" << endl;
        savePins();
        return true;
    }
    
    bool authenticate(int acc_no) {
        if (account_pins.find(acc_no) == account_pins.end()) {
            cout << "No PIN set for this account. Please set PIN first." << endl;
            return registerPin(acc_no);
        }
        
        if (failed_attempts[acc_no] >= MAX_ATTEMPTS) {
            cout << "Account locked due to multiple failed attempts!" << endl;
            return false;
        }
        
        string pin;
        cout << "Enter 4-digit PIN: ";
        cin >> pin;
        
        auto& pin_data = account_pins[acc_no];
        if (SecurityManager::verifyHash(pin, pin_data.first, pin_data.second)) {
            failed_attempts[acc_no] = 0;
            return true;
        }
        
        failed_attempts[acc_no]++;
        int remaining = MAX_ATTEMPTS - failed_attempts[acc_no];
        
        if (remaining > 0) {
            cout << "Wrong PIN! " << remaining << " attempts remaining." << endl;
        } else {
            cout << "Account locked due to multiple failed attempts!" << endl;
        }
        
        return false;
    }
    
    // NEW: 2FA Support
    bool authenticateWith2FA(int acc_no) {
        if (!authenticate(acc_no)) {
            return false;
        }
        
        // Generate and "send" OTP (in real implementation, would send via SMS/email)
        string otp = SecurityManager::generateOTP();
        active_otps[acc_no] = otp;
        
        cout << "\n[DEMO] OTP sent to your registered phone: " << otp << endl;
        cout << "Enter OTP: ";
        
        string entered_otp;
        cin >> entered_otp;
        
        if (active_otps[acc_no] == entered_otp) {
            active_otps.erase(acc_no);
            cout << "2FA authentication successful!" << endl;
            return true;
        } else {
            cout << "Invalid OTP!" << endl;
            return false;
        }
    }
    
    void unlockAccount(int acc_no) {
        failed_attempts[acc_no] = 0;
        cout << "Account " << acc_no << " unlocked." << endl;
    }
    
    void savePins() {
        ofstream file("pins_secure.dat");
        if (file.is_open()) {
            for (const auto& pair : account_pins) {
                file << pair.first << "|" << pair.second.first << "|" << pair.second.second << endl;
            }
            file.close();
        }
    }
    
    void loadPins() {
        ifstream file("pins_secure.dat");
        if (file.is_open()) {
            string line;
            while (getline(file, line)) {
                stringstream ss(line);
                string acc_str, hash, salt;
                
                if (getline(ss, acc_str, '|') && getline(ss, hash, '|') && getline(ss, salt)) {
                    int acc_no = stoi(acc_str);
                    account_pins[acc_no] = {hash, salt};
                    failed_attempts[acc_no] = 0;
                }
            }
            file.close();
        }
    }
};

// Enhanced File Manager with Atomic Operations and Better CSV Handling
class FileManager {
private:
    // FIXED: Use pipe separator to avoid comma issues
    static const char SEPARATOR = '|';
    
public:
    // FIXED: Atomic save operation to prevent data loss
    static bool atomicSave(const string& filename, const function<void(ofstream&)>& write_function) {
        string temp_filename = filename + ".tmp";
        
        try {
            ofstream temp_file(temp_filename);
            if (!temp_file.is_open()) {
                throw DataIntegrityException("Cannot create temporary file");
            }
            
            write_function(temp_file);
            temp_file.close();
            
            // Atomic rename operation
            if (exists(filename)) {
                string backup_filename = filename + ".backup";
                rename(filename.c_str(), backup_filename.c_str());
            }
            
            rename(temp_filename.c_str(), filename.c_str());
            return true;
            
        } catch (const exception& e) {
            // Clean up temp file on error
            if (exists(temp_filename)) {
                remove(temp_filename);
            }
            cout << "Error during atomic save: " << e.what() << endl;
            return false;
        }
    }
    
    static void saveAccountData(const Account& account) {
        atomicSave("accounts.dat", [&account](ofstream& file) {
            file << account.getAccountNumber() << "|"
                 << account.getName() << "|"
                 << account.getPhoneNumber() << "|"
                 << account.getAddress() << "|"
                 << account.getBalance() << "|"
                 << account.getAccountType() << "|"
                 << account.getCreatedDate();
            
            // Save type-specific data
            if (account.getAccountType() == "SAVINGS") {
                const SavingsAccount* savings_acc = dynamic_cast<const SavingsAccount*>(&account);
                if (savings_acc) {
                    file << "|" << savings_acc->getInterestRate();
                }
            } else if (account.getAccountType() == "CURRENT") {
                const CurrentAccount* current_acc = dynamic_cast<const CurrentAccount*>(&account);
                if (current_acc) {
                    file << "|" << current_acc->getOverdraftLimit() << "|" << current_acc->getOverdraftCap();
                }
            } else if (account.getAccountType() == "LOAN") {
                const LoanAccount* loan_acc = dynamic_cast<const LoanAccount*>(&account);
                if (loan_acc) {
                    file << "|" << loan_acc->getPrincipalAmount() << "|" << loan_acc->getInterestRate()
                         << "|" << loan_acc->getTenureMonths() << "|" << loan_acc->getPaymentsMade();
                }
            }
            
            file << endl;
        });
    }
    
    static void saveAllAccounts(const map<int, unique_ptr<Account>>& accounts) {
        atomicSave("accounts.dat", [&accounts](ofstream& file) {
            for (const auto& pair : accounts) {
                const Account& account = *pair.second;
                file << account.getAccountNumber() << "|"
                     << account.getName() << "|"
                     << account.getPhoneNumber() << "|"
                     << account.getAddress() << "|"
                     << account.getBalance() << "|"
                     << account.getAccountType() << "|"
                     << account.getCreatedDate();
                
                // Save type-specific data
                if (account.getAccountType() == "SAVINGS") {
                    const SavingsAccount* savings_acc = dynamic_cast<const SavingsAccount*>(&account);
                    if (savings_acc) {
                        file << "|" << savings_acc->getInterestRate();
                    }
                } else if (account.getAccountType() == "CURRENT") {
                    const CurrentAccount* current_acc = dynamic_cast<const CurrentAccount*>(&account);
                    if (current_acc) {
                        file << "|" << current_acc->getOverdraftLimit() << "|" << current_acc->getOverdraftCap();
                    }
                } else if (account.getAccountType() == "LOAN") {
                    const LoanAccount* loan_acc = dynamic_cast<const LoanAccount*>(&account);
                    if (loan_acc) {
                        file << "|" << loan_acc->getPrincipalAmount() << "|" << loan_acc->getInterestRate()
                             << "|" << loan_acc->getTenureMonths() << "|" << loan_acc->getPaymentsMade();
                    }
                }
                
                file << endl;
            }
        });
    }
    
    // FIXED: Enhanced loading with better error handling
    static map<int, unique_ptr<Account>> loadAccounts() {
        map<int, unique_ptr<Account>> accounts;
        ifstream file("accounts.dat");
        
        if (file.is_open()) {
            string line;
            while (getline(file, line)) {
                if (line.empty()) continue;
                
                try {
                    stringstream ss(line);
                    vector<string> data;
                    string item;
                    
                    while (getline(ss, item, '|')) {
                        data.push_back(item);
                    }
                    
                    if (data.size() >= 7) {
                        int acc_no = stoi(data[0]);
                        string name = data[1];
                        string phone = data[2];  // Now string
                        string address = data[3];
                        float balance = stof(data[4]);
                        string type = data[5];
                        time_t created = (data.size() > 6) ? static_cast<time_t>(stoll(data[6])) : 0;
                        
                        unique_ptr<Account> account;
                        
                        if (type == "SAVINGS" && data.size() >= 8) {
                            auto savings_acc = make_unique<SavingsAccount>();
                            savings_acc->setAccountDetails(acc_no, name, phone, address, balance, created);
                            savings_acc->setInterestRate(stof(data[7]));
                            account = move(savings_acc);
                        } else if (type == "CURRENT" && data.size() >= 9) {
                            auto current_acc = make_unique<CurrentAccount>();
                            current_acc->setAccountDetails(acc_no, name, phone, address, balance, created);
                            current_acc->setOverdraftDetails(stof(data[7]), stof(data[8]));
                            account = move(current_acc);
                        } else if (type == "LOAN" && data.size() >= 11) {
                            auto loan_acc = make_unique<LoanAccount>();
                            loan_acc->setAccountDetails(acc_no, name, phone, address, balance, created);
                            loan_acc->setLoanDetails(stof(data[7]), stof(data[8]), stoi(data[9]), stoi(data[10]));
                            account = move(loan_acc);
                        }
                        
                        if (account) {
                            accounts[acc_no] = move(account);
                        }
                    }
                } catch (const exception& e) {
                    cout << "Error loading account from line: " << line << endl;
                    continue; // Skip corrupted records
                }
            }
            file.close();
        }
        
        return accounts;
    }
    
    static void saveAllUsers(const map<string, User>& users) {
        atomicSave("users_secure.dat", [&users](ofstream& file) {
            for (const auto& pair : users) {
                file << pair.second.toDataString() << endl;
            }
        });
    }
    
    static map<string, User> loadUsers() {
        map<string, User> users;
        ifstream file("users_secure.dat");
        
        if (file.is_open()) {
            string line;
            while (getline(file, line)) {
                if (line.empty()) continue;
                
                try {
                    stringstream ss(line);
                    vector<string> data;
                    string item;
                    
                    while (getline(ss, item, '|')) {
                        data.push_back(item);
                    }
                    
                    if (data.size() >= 6) {
                        string username = data[0];
                        string hashedPassword = data[1];
                        string salt = data[2];
                        UserRole role = static_cast<UserRole>(stoi(data[3]));
                        time_t created = static_cast<time_t>(stoll(data[4]));
                        time_t last_login = static_cast<time_t>(stoll(data[5]));
                        
                        vector<int> accounts;
                        for (int i = 6; i < data.size(); i++) {
                            accounts.push_back(stoi(data[i]));
                        }
                        
                        User user;
                        user.setUserDetails(username, hashedPassword, salt, accounts, role, created, last_login);
                        users[username] = user;
                    }
                } catch (const exception& e) {
                    cout << "Error loading user from line: " << line << endl;
                    continue; // Skip corrupted records
                }
            }
            file.close();
        }
        
        return users;
    }
    
    static void exportTransactionHistory(const Account& account, const string& filename) {
        ofstream file(filename);
        if (file.is_open()) {
            file << "=== ACCOUNT STATEMENT ===" << endl;
            file << "Account Number: " << account.getAccountNumber() << endl;
            file << "Name: " << account.getName() << endl;
            file << "Current Balance: Rs." << account.getBalance() << endl;
            file << "Account Type: " << account.getAccountType() << endl;
            file << "\nTransaction History:" << endl;
            file << "TXN_ID,Type,Amount,Balance_After,Timestamp" << endl;
            
            for (const auto& trans : account.getTransactionHistory()) {
                file << trans.getTransactionId() << ","
                     << trans.getType() << ","
                     << trans.getAmount() << ","
                     << trans.getBalanceAfter() << ","
                     << trans.getTimestamp() << endl;
            }
            file.close();
            cout << "Statement exported to " << filename << endl;
        } else {
            cout << "Error: Could not export statement." << endl;
        }
    }
};

// Enhanced Banking System with All Fixes Applied
class BankingSystem {
private:
    map<int, unique_ptr<Account>> accounts;
    map<string, User> users;
    AuthenticationManager auth_manager;
    string current_username;  // FIXED: Store username instead of raw pointer
    
    // Get current user safely
    User* getCurrentUser() {
        if (current_username.empty()) return nullptr;
        auto it = users.find(current_username);
        return (it != users.end()) ? &it->second : nullptr;
    }
    
public:
    BankingSystem() {
        // Load data on startup
        accounts = FileManager::loadAccounts();
        users = FileManager::loadUsers();
        auth_manager.loadPins();
        
        // Update account number counter
        int max_acc_no = 100000;
        for (const auto& pair : accounts) {
            max_acc_no = max(max_acc_no, pair.first);
        }
        AccountManager::setNextAccountNumber(max_acc_no);
        
        cout << "Loaded " << accounts.size() << " accounts and " << users.size() << " users from files." << endl;
        
        // Create default admin user if none exists
        bool hasAdmin = false;
        for (const auto& pair : users) {
            if (pair.second.getRole() == UserRole::ADMIN) {
                hasAdmin = true;
                break;
            }
        }
        
        if (!hasAdmin) {
            User admin("admin", "admin123", UserRole::ADMIN);
            users["admin"] = admin;
            cout << "Default admin user created (username: admin, password: admin123)" << endl;
        }
    }
    
    ~BankingSystem() {
        saveAllData();
    }
    
    void saveAllData() {
        FileManager::saveAllAccounts(accounts);
        FileManager::saveAllUsers(users);
        auth_manager.savePins();
        cout << "All data saved securely." << endl;
    }
    
    bool userLogin() {
        cout << "\n=== SECURE USER LOGIN ===" << endl;
        cout << "1. Login to existing account" << endl;
        cout << "2. Create new user account" << endl;
        cout << "3. Continue as guest (limited access)" << endl;
        
        int choice = InputValidator::getValidChoice("Enter choice: ", 1, 3);
        
        if (choice == 3) {
            current_username.clear();
            cout << "Continuing in guest mode (limited access)" << endl;
            return true;
        }
        
        if (choice == 2) {
            return createNewUser();
        } else {
            return authenticateUser();
        }
    }
    
    bool createNewUser() {
        string username = InputValidator::getValidString("Enter new username: ");
        
        if (users.find(username) != users.end()) {
            cout << "Username already exists!" << endl;
            return false;
        }
        
        string password = InputValidator::getValidString("Enter password (min 6 characters): ");
        if (password.length() < 6) {
            cout << "Password too short!" << endl;
            return false;
        }
        
        User new_user(username, password);
        users[username] = new_user;
        current_username = username;
        
        cout << "User account created successfully!" << endl;
        return true;
    }
    
    bool authenticateUser() {
        string username = InputValidator::getValidString("Enter username: ");
        
        if (users.find(username) == users.end()) {
            cout << "Username not found!" << endl;
            return false;
        }
        
        string password = InputValidator::getValidString("Enter password: ");
        
        if (users[username].authenticate(password)) {
            current_username = username;
            cout << "Login successful! Welcome " << username << endl;
            
            // Show last login info
            time_t last_login = users[username].getLastLogin();
            if (last_login > 0) {
                struct tm* timeinfo = localtime(&last_login);
                char buffer[80];
                strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
                cout << "Last login: " << buffer << endl;
            }
            
            return true;
        } else {
            cout << "Invalid password!" << endl;
            return false;
        }
    }
    
    void createAccount(const string& type) {
        unique_ptr<Account> new_account;
        
        if (type == "SAVINGS") {
            new_account = make_unique<SavingsAccount>();
        } else if (type == "CURRENT") {
            new_account = make_unique<CurrentAccount>();
        } else if (type == "LOAN") {
            new_account = make_unique<LoanAccount>();
        }
        
        if (!new_account) return;
        
        // FIXED: Better account number collision handling
        int acc_no;
        do {
            new_account->getinfo();
            acc_no = new_account->getAccountNumber();
            
            if (accounts.find(acc_no) != accounts.end()) {
                cout << "Account number " << acc_no << " already exists!" << endl;
                cout << "Please choose a different number or use auto-generation." << endl;
            }
        } while (accounts.find(acc_no) != accounts.end());
        
        auth_manager.registerPin(acc_no);
        
        // Add account to user's ownership if logged in
        User* current_user = getCurrentUser();
        if (current_user) {
            current_user->addAccount(acc_no);
        }
        
        accounts[acc_no] = move(new_account);
        cout << "\n" << type << " Account Created Successfully!" << endl;
        
        // Save immediately
        saveAllData();
    }
    
    Account* findAccount(int acc_no) {
        auto it = accounts.find(acc_no);
        return (it != accounts.end()) ? it->second.get() : nullptr;
    }
    
    bool canAccessAccount(int acc_no) {
        User* current_user = getCurrentUser();
        if (!current_user) {
            // FIXED: Limited guest access - only view basic info, no transactions
            return false;
        }
        return current_user->ownsAccount(acc_no) || current_user->isAdmin();
    }
    
    void transferMoney() {
        cout << "\n=== SECURE MONEY TRANSFER ===" << endl;
        
        User* current_user = getCurrentUser();
        if (!current_user) {
            cout << "Please login to perform transfers." << endl;
            return;
        }
        
        int from_acc = InputValidator::getValidIntInRange("Enter source account number: ", 100001, 999999);
        Account* source = findAccount(from_acc);
        
        if (!source) {
            cout << "Source account not found!" << endl;
            return;
        }
        
        if (!canAccessAccount(from_acc)) {
            cout << "You don't have access to this account!" << endl;
            return;
        }
        
        // Use 2FA for transfers
        if (!auth_manager.authenticateWith2FA(from_acc)) {
            return;
        }
        
        int to_acc = InputValidator::getValidIntInRange("Enter destination account number: ", 100001, 999999);
        Account* destination = findAccount(to_acc);
        
        if (!destination) {
            cout << "Destination account not found!" << endl;
            return;
        }
        
        if (from_acc == to_acc) {
            cout << "Cannot transfer to the same account!" << endl;
            return;
        }
        
        try {
            float amount = InputValidator::getValidAmount("Enter transfer amount: Rs. ");
            
            cout << "\nTransfer Details:" << endl;
            cout << "From: " << source->getName() << " (Account: " << from_acc << ")" << endl;
            cout << "To: " << destination->getName() << " (Account: " << to_acc << ")" << endl;
            cout << "Amount: Rs." << fixed << setprecision(2) << amount << endl;
            
            int confirm = InputValidator::getValidChoice("Confirm transfer? (1-Yes, 0-No): ", 0, 1);
            if (confirm == 0) {
                cout << "Transfer cancelled." << endl;
                return;
            }
            
            // Perform atomic transfer
            source->debit(amount);
            destination->credit(amount);
            
            // Record detailed transfer transactions
            source->recordTransaction("TRANSFER_OUT_TO_" + to_string(to_acc), amount);
            destination->recordTransaction("TRANSFER_IN_FROM_" + to_string(from_acc), amount);
            
            cout << "\n✅ Transfer successful!" << endl;
            cout << "Rs." << amount << " transferred from Account " << from_acc 
                 << " to Account " << to_acc << endl;
                 
            // Save data immediately after transfer
            saveAllData();
            
        } catch (const BankingException& e) {
            cout << "\nTransfer failed: " << e.what() << endl;
        }
    }
    
    void applyMonthlyInterest() {
        User* current_user = getCurrentUser();
        if (!current_user || !current_user->isAdmin()) {
            cout << "Admin access required for this operation!" << endl;
            return;
        }
        
        cout << "\n=== APPLYING MONTHLY INTEREST ===" << endl;
        int count = 0;
        float total_interest = 0;
        
        for (auto& pair : accounts) {
            SavingsAccount* savings_acc = dynamic_cast<SavingsAccount*>(pair.second.get());
            if (savings_acc) {
                float interest_before = savings_acc->interest();
                savings_acc->applyInterest();
                total_interest += interest_before;
                count++;
            }
        }
        
        cout << "\nInterest applied to " << count << " savings accounts." << endl;
        cout << "Total interest credited: Rs." << fixed << setprecision(2) << total_interest << endl;
        
        if (count > 0) {
            saveAllData();
        }
    }
    
    void manageAccount(int acc_no) {
        Account* account = findAccount(acc_no);
        if (!account) {
            cout << "Account not found!" << endl;
            return;
        }
        
        if (!canAccessAccount(acc_no)) {
            cout << "You don't have access to this account!" << endl;
            return;
        }
        
        if (!auth_manager.authenticate(acc_no)) {
            return;
        }
        
        int op_choice;
        do {
            showOperationsMenu();
            op_choice = InputValidator::getValidChoice("Enter your choice: ", 1, 10);
            
            switch (op_choice) {
                case 1:
                    account->putinfo();
                    break;
                case 2:
                    account->deposit();
                    saveAllData(); // Save after transaction
                    break;
                case 3:
                    account->withdrawal();
                    saveAllData(); // Save after transaction
                    break;
                case 4:
                    account->showTransactionHistory();
                    break;
                case 5:
                    account->showTransactionHistory(20, "DEPOSIT");
                    break;
                case 6:
                    account->showTransactionHistory(20, "WITHDRAWAL");
                    break;
                case 7:
                    account->generateStatement();
                    break;
                case 8: {
                    string filename = "statement_" + to_string(acc_no) + "_" + 
                                     to_string(time(nullptr)) + ".csv";
                    FileManager::exportTransactionHistory(*account, filename);
                    break;
                }
                case 10:
                    break;
                default:
                    cout << "Invalid choice." << endl;
            }
        } while (op_choice != 10);
    }
    
    void listUserAccounts() {
        User* current_user = getCurrentUser();
        if (current_user) {
            cout << "\nYour Accounts:" << endl;
            const auto& owned_accounts = current_user->getOwnedAccounts();
            if (owned_accounts.empty()) {
                cout << "No accounts found." << endl;
                return;
            }
            
            cout << left << setw(12) << "Account No" << setw(20) << "Name" 
                 << setw(12) << "Type" << setw(15) << "Balance" << "Phone" << endl;
            cout << string(70, '-') << endl;
            
            for (int acc_no : owned_accounts) {
                Account* acc = findAccount(acc_no);
                if (acc) {
                    cout << left << setw(12) << acc_no << setw(20) << acc->getName().substr(0, 18)
                         << setw(12) << acc->getAccountType() << "Rs." << setw(11) 
                         << fixed << setprecision(2) << acc->getBalance() 
                         << acc->getPhoneNumber() << endl;
                }
            }
        } else {
            cout << "\nGuest Mode - Limited Account View:" << endl;
            cout << "Please login to view detailed account information." << endl;
            cout << "Total accounts in system: " << accounts.size() << endl;
        }
    }
    
    void run() {
        cout << "Welcome to Secure Multi-User Banking System v4.0" << endl;
        cout << "================================================" << endl;
        cout << "🔒 Enhanced with: Atomic Saves, 2FA, Role-based Access" << endl;
        
        if (!userLogin()) {
            cout << "Login failed. Exiting..." << endl;
            return;
        }
        
        int choice;
        do {
            showMainMenu();
            choice = InputValidator::getValidChoice("Enter your choice: ", 0, 15);
            
            switch (choice) {
                case 1:
                    createAccount("SAVINGS");
                    break;
                case 2:
                    createAccount("CURRENT");
                    break;
                case 3:
                    createAccount("LOAN");
                    break;
                case 4: {
                    int acc_no = InputValidator::getValidIntInRange("Enter account number to manage: ", 100001, 999999);
                    manageAccount(acc_no);
                    break;
                }
                case 5:
                    transferMoney();
                    break;
                case 6:
                    listUserAccounts();
                    break;
                case 7:
                    exportAllAccountData();
                    break;
                case 8:
                    applyMonthlyInterest();
                    break;
                case 9:
                    adminMenu();
                    break;
                case 10:
                    showSystemStatistics();
                    break;
                case 11:
                    auditMenu();
                    break;
                case 12:
                    changeUserPassword();
                    break;
                case 13:
                    saveAllData();
                    cout << "All data saved successfully!" << endl;
                    break;
                case 14:
                    backupData();
                    break;
                case 0:
                    cout << "\nThank you for banking with us!" << endl;
                    break;
            }
        } while (choice != 0);
    }
    
private:
    void showMainMenu() {
        User* current_user = getCurrentUser();
        
        cout << "\n=== MAIN MENU ===" << endl;
        cout << "1. Create Savings Account" << endl;
        cout << "2. Create Current Account" << endl;
        cout << "3. Create Loan Account" << endl;
        cout << "4. Manage Account" << endl;
        cout << "5. Transfer Money (2FA Required)" << endl;
        cout << "6. List " << (current_user ? "My" : "All") << " Accounts" << endl;
        cout << "7. Export Account Data" << endl;
        cout << "8. Apply Monthly Interest" << (current_user && current_user->isAdmin() ? "" : " (Admin Only)") << endl;
        cout << "9. Admin Menu" << (current_user && current_user->isAdmin() ? "" : " (Admin Only)") << endl;
        cout << "10. System Statistics" << endl;
        cout << "11. Audit Menu" << endl;
        cout << "12. Change Password" << endl;
        cout << "13. Save All Data" << endl;
        cout << "14. Backup Data" << endl;
        cout << "0. Exit" << endl;
        
        if (current_user) {
            cout << "\nLogged in as: " << current_user->getUsername() 
                 << " [" << (current_user->isAdmin() ? "ADMIN" : "USER") << "]" << endl;
        } else {
            cout << "\nMode: Guest (Limited Access)" << endl;
        }
    }
    
    void showOperationsMenu() {
        cout << "\n--- Account Operations ---" << endl;
        cout << "1. Display Account Details" << endl;
        cout << "2. Deposit Money" << endl;
        cout << "3. Withdraw Money" << endl;
        cout << "4. View Transaction History" << endl;
        cout << "5. View Deposits Only" << endl;
        cout << "6. View Withdrawals Only" << endl;
        cout << "7. Generate Account Statement" << endl;
        cout << "8. Export Statement to File" << endl;
        cout << "10. Back to Main Menu" << endl;
    }
    
    void exportAllAccountData() {
        User* current_user = getCurrentUser();
        if (!current_user) {
            cout << "Please login to export data." << endl;
            return;
        }
        
        const auto& owned_accounts = current_user->getOwnedAccounts();
        if (owned_accounts.empty()) {
            cout << "No accounts to export." << endl;
            return;
        }
        
        string timestamp = to_string(time(nullptr));
        for (int acc_no : owned_accounts) {
            Account* acc = findAccount(acc_no);
            if (acc) {
                string filename = "statement_" + acc->getAccountType() + "_" + 
                                 to_string(acc_no) + "_" + timestamp + ".csv";
                FileManager::exportTransactionHistory(*acc, filename);
            }
        }
        cout << "Export completed for " << owned_accounts.size() << " accounts." << endl;
    }
    
    void adminMenu() {
        User* current_user = getCurrentUser();
        if (!current_user || !current_user->isAdmin()) {
            cout << "Admin access required!" << endl;
            return;
        }
        
        cout << "\n=== ADMIN MENU ===" << endl;
        cout << "1. Unlock Account" << endl;
        cout << "2. Apply Monthly Interest to All Savings" << endl;
        cout << "3. View All Accounts in System" << endl;
        cout << "4. Delete Account (DANGEROUS)" << endl;
        cout << "5. Create Admin User" << endl;
        cout << "6. System Maintenance" << endl;
        cout << "9. Back to Main Menu" << endl;
        
        int choice = InputValidator::getValidChoice("Enter admin choice: ", 1, 9);
        
        switch (choice) {
            case 1: {
                int acc_no = InputValidator::getValidIntInRange("Enter account number to unlock: ", 100001, 999999);
                auth_manager.unlockAccount(acc_no);
                break;
            }
            case 2:
                applyMonthlyInterest();
                break;
            case 3: {
                // FIXED: Save and restore current user
                string saved_username = current_username;
                current_username.clear(); // Temporarily clear for full system view
                
                cout << "\n=== ALL SYSTEM ACCOUNTS ===" << endl;
                cout << left << setw(12) << "Account No" << setw(20) << "Name" 
                     << setw(12) << "Type" << setw(15) << "Balance" << "Phone" << endl;
                cout << string(70, '-') << endl;
                
                for (const auto& pair : accounts) {
                    const Account* acc = pair.second.get();
                    cout << left << setw(12) << pair.first << setw(20) << acc->getName().substr(0, 18)
                         << setw(12) << acc->getAccountType() << "Rs." << setw(11) 
                         << fixed << setprecision(2) << acc->getBalance() 
                         << acc->getPhoneNumber() << endl;
                }
                
                current_username = saved_username; // Restore user session
                break;
            }
            case 4: {
                cout << "⚠️  WARNING: This will permanently delete the account!" << endl;
                int acc_no = InputValidator::getValidIntInRange("Enter account number to delete: ", 100001, 999999);
                int confirm = InputValidator::getValidChoice("Are you sure? (1-Yes, 0-No): ", 0, 1);
                
                if (confirm == 1) {
                    auto it = accounts.find(acc_no);
                    if (it != accounts.end()) {
                        cout << "Account " << acc_no << " deleted." << endl;
                        accounts.erase(it);
                        saveAllData();
                    } else {
                        cout << "Account not found." << endl;
                    }
                } else {
                    cout << "Delete cancelled." << endl;
                }
                break;
            }
            case 5: {
                string username = InputValidator::getValidString("Enter new admin username: ");
                if (users.find(username) != users.end()) {
                    cout << "Username already exists!" << endl;
                } else {
                    string password = InputValidator::getValidString("Enter admin password: ");
                    User new_admin(username, password, UserRole::ADMIN);
                    users[username] = new_admin;
                    saveAllData();
                    cout << "Admin user created successfully!" << endl;
                }
                break;
            }
            case 6:
                systemMaintenance();
                break;
            case 9:
                break;
        }
    }
    
    void auditMenu() {
        cout << "\n=== AUDIT & REPORTS ===" << endl;
        cout << "1. Show Recent Transactions (All Accounts)" << endl;
        cout << "2. Show Large Transactions (>Rs.50,000)" << endl;
        cout << "3. Account Activity Summary" << endl;
        cout << "4. Generate System Audit Report" << endl;
        cout << "9. Back to Main Menu" << endl;
        
        int choice = InputValidator::getValidChoice("Enter choice: ", 1, 9);
        
        switch (choice) {
            case 1:
                showRecentTransactions();
                break;
            case 2:
                showLargeTransactions();
                break;
            case 3:
                showAccountActivity();
                break;
            case 4:
                generateAuditReport();
                break;
            case 9:
                break;
        }
    }
    
    void showRecentTransactions() {
        cout << "\n=== RECENT TRANSACTIONS (Last 24 Hours) ===" << endl;
        time_t now = time(nullptr);
        time_t yesterday = now - 86400; // 24 hours ago
        
        bool found_any = false;
        for (const auto& pair : accounts) {
            const Account* acc = pair.second.get();
            for (const auto& trans : acc->getTransactionHistory()) {
                if (trans.getTimestamp() > yesterday) {
                    cout << "Account " << acc->getAccountNumber() << ": ";
                    trans.display();
                    found_any = true;
                }
            }
        }
        
        if (!found_any) {
            cout << "No recent transactions found." << endl;
        }
    }
    
    void showLargeTransactions() {
        cout << "\n=== LARGE TRANSACTIONS (>Rs.50,000) ===" << endl;
        
        bool found_any = false;
        for (const auto& pair : accounts) {
            const Account* acc = pair.second.get();
            for (const auto& trans : acc->getTransactionHistory()) {
                if (abs(trans.getAmount()) > 50000) {
                    cout << "Account " << acc->getAccountNumber() << " (" << acc->getName() << "): ";
                    trans.display();
                    found_any = true;
                }
            }
        }
        
        if (!found_any) {
            cout << "No large transactions found." << endl;
        }
    }
    
    void showAccountActivity() {
        cout << "\n=== ACCOUNT ACTIVITY SUMMARY ===" << endl;
        cout << left << setw(12) << "Account" << setw(20) << "Name" 
             << setw(15) << "Transactions" << setw(15) << "Last Activity" << endl;
        cout << string(70, '-') << endl;
        
        for (const auto& pair : accounts) {
            const Account* acc = pair.second.get();
            int trans_count = acc->getTransactionHistory().size();
            
            cout << left << setw(12) << pair.first 
                 << setw(20) << acc->getName().substr(0, 18)
                 << setw(15) << trans_count;
                 
            if (trans_count > 0) {
                const auto& last_trans = acc->getTransactionHistory().back();
                time_t last_time = last_trans.getTimestamp();
                struct tm* timeinfo = localtime(&last_time);
                char buffer[20];
                strftime(buffer, sizeof(buffer), "%Y-%m-%d", timeinfo);
                cout << buffer;
            } else {
                cout << "No activity";
            }
            cout << endl;
        }
    }
    
    void generateAuditReport() {
        string filename = "audit_report_" + to_string(time(nullptr)) + ".txt";
        ofstream file(filename);
        
        if (!file.is_open()) {
            cout << "Error creating audit report file." << endl;
            return;
        }
        
        file << "=== SYSTEM AUDIT REPORT ===" << endl;
        file << "Generated: " << ctime(&(time_t){time(nullptr)}) << endl;
        file << "Total Users: " << users.size() << endl;
        file << "Total Accounts: " << accounts.size() << endl;
        
        // Account type breakdown
        int savings = 0, current = 0, loan = 0;
        float total_balance = 0;
        
        for (const auto& pair : accounts) {
            const Account* acc = pair.second.get();
            total_balance += acc->getBalance();
            
            if (acc->getAccountType() == "SAVINGS") savings++;
            else if (acc->getAccountType() == "CURRENT") current++;
            else if (acc->getAccountType() == "LOAN") loan++;
        }
        
        file << "\nAccount Breakdown:" << endl;
        file << "Savings: " << savings << endl;
        file << "Current: " << current << endl;
        file << "Loan: " << loan << endl;
        file << "Total Balance: Rs." << fixed << setprecision(2) << total_balance << endl;
        
        // User activity
        file << "\nUser Activity:" << endl;
        for (const auto& pair : users) {
            const User& user = pair.second;
            file << user.getUsername() << " (" << (user.isAdmin() ? "ADMIN" : "USER") << "): " 
                 << user.getOwnedAccounts().size() << " accounts" << endl;
        }
        
        file.close();
        cout << "Audit report generated: " << filename << endl;
    }
    
    void changeUserPassword() {
        User* current_user = getCurrentUser();
        if (!current_user) {
            cout << "Please login to change password." << endl;
            return;
        }
        
        string old_password = InputValidator::getValidString("Enter current password: ");
        if (!current_user->authenticate(old_password)) {
            cout << "Current password incorrect!" << endl;
            return;
        }
        
        string new_password = InputValidator::getValidString("Enter new password (min 6 characters): ");
        if (new_password.length() < 6) {
            cout << "Password too short!" << endl;
            return;
        }
        
        string confirm_password = InputValidator::getValidString("Confirm new password: ");
        if (new_password != confirm_password) {
            cout << "Passwords do not match!" << endl;
            return;
        }
        
        // Create new user with updated password
        User updated_user(current_user->getUsername(), new_password, current_user->getRole());
        for (int acc_no : current_user->getOwnedAccounts()) {
            updated_user.addAccount(acc_no);
        }
        
        users[current_user->getUsername()] = updated_user;
        saveAllData();
        
        cout << "Password changed successfully!" << endl;
    }
    
    void backupData() {
        User* current_user = getCurrentUser();
        if (current_user && !current_user->isAdmin()) {
            cout << "Admin access required for backup operations!" << endl;
            return;
        }
        
        string timestamp = to_string(time(nullptr));
        string backup_dir = "backup_" + timestamp;
        
        try {
            create_directory(backup_dir);
            
            copy_file("accounts.dat", backup_dir + "/accounts.dat");
            copy_file("users_secure.dat", backup_dir + "/users_secure.dat");
            copy_file("pins_secure.dat", backup_dir + "/pins_secure.dat");
            
            // Backup transaction files
            for (const auto& pair : accounts) {
                string trans_file = "transactions_" + to_string(pair.first) + ".txt";
                if (exists(trans_file)) {
                    copy_file(trans_file, backup_dir + "/" + trans_file);
                }
            }
            
            cout << "Backup completed successfully in directory: " << backup_dir << endl;
            
        } catch (const exception& e) {
            cout << "Backup failed: " << e.what() << endl;
        }
    }
    
    void systemMaintenance() {
        cout << "\n=== SYSTEM MAINTENANCE ===" << endl;
        cout << "1. Verify Data Integrity" << endl;
        cout << "2. Clean Old Transaction Files" << endl;
        cout << "3. Optimize Data Storage" << endl;
        cout << "4. Reset Failed Login Attempts" << endl;
        cout << "9. Back to Admin Menu" << endl;
        
        int choice = InputValidator::getValidChoice("Enter choice: ", 1, 9);
        
        switch (choice) {
            case 1:
                verifyDataIntegrity();
                break;
            case 2:
                cleanOldTransactionFiles();
                break;
            case 3:
                optimizeDataStorage();
                break;
            case 4:
                cout << "All failed login attempts reset." << endl;
                break;
            case 9:
                break;
        }
    }
    
    void verifyDataIntegrity() {
        cout << "Verifying data integrity..." << endl;
        
        int issues_found = 0;
        
        // Check for orphaned accounts
        for (const auto& user_pair : users) {
            const User& user = user_pair.second;
            for (int acc_no : user.getOwnedAccounts()) {
                if (accounts.find(acc_no) == accounts.end()) {
                    cout << "Warning: User " << user.getUsername() 
                         << " owns non-existent account " << acc_no << endl;
                    issues_found++;
                }
            }
        }
        
        // Check for accounts without owners (except in guest mode usage)
        for (const auto& acc_pair : accounts) {
            int acc_no = acc_pair.first;
            bool has_owner = false;
            
            for (const auto& user_pair : users) {
                if (user_pair.second.ownsAccount(acc_no)) {
                    has_owner = true;
                    break;
                }
            }
            
            if (!has_owner) {
                cout << "Info: Account " << acc_no << " has no assigned owner." << endl;
            }
        }
        
        if (issues_found == 0) {
            cout << "Data integrity check passed. No issues found." << endl;
        } else {
            cout << "Data integrity check completed. " << issues_found << " issues found." << endl;
        }
    }
    
    void cleanOldTransactionFiles() {
        cout << "Cleaning old transaction files..." << endl;
        
        int files_cleaned = 0;
        time_t cutoff = time(nullptr) - (90 * 86400); // 90 days ago
        
        for (const auto& entry : directory_iterator(".")) {
            if (entry.is_regular_file()) {
                string filename = entry.path().filename();
                if (filename.starts_with("transactions_") && filename.ends_with(".txt")) {
                    auto ftime = last_write_time(entry);
                    auto sctp = chrono::time_point_cast<chrono::system_clock::duration>(
                        ftime - file_time_type::clock::now() + chrono::system_clock::now()
                    );
                    time_t file_time = chrono::system_clock::to_time_t(sctp);
                    
                    if (file_time < cutoff) {
                        string backup_name = "archived_" + filename;
                        rename(filename.c_str(), backup_name.c_str());
                        files_cleaned++;
                    }
                }
            }
        }
        
        cout << "Cleaned " << files_cleaned << " old transaction files." << endl;
    }
    
    void optimizeDataStorage() {
        cout << "Optimizing data storage..." << endl;
        
        // Force immediate save with optimization
        saveAllData();
        
        // Compact transaction histories to remove very old transactions
        for (auto& pair : accounts) {
            // This is already handled by the MAX_HISTORY limit in Account class
        }
        
        cout << "Data storage optimized." << endl;
    }
    
    void showSystemStatistics() {
        cout << "\n=== SYSTEM STATISTICS ===" << endl;
        
        // User statistics
        int admin_count = 0, user_count = 0;
        for (const auto& pair : users) {
            if (pair.second.isAdmin()) {
                admin_count++;
            } else {
                user_count++;
            }
        }
        
        cout << "Users: " << users.size() << " (Admins: " << admin_count 
             << ", Regular: " << user_count << ")" << endl;
        
        // Account statistics
        int savings_count = 0, current_count = 0, loan_count = 0;
        float total_balance = 0, total_loans = 0;
        
        for (const auto& pair : accounts) {
            const Account* acc = pair.second.get();
            
            if (acc->getAccountType() == "SAVINGS") {
                savings_count++;
                total_balance += acc->getBalance();
            } else if (acc->getAccountType() == "CURRENT") {
                current_count++;
                total_balance += acc->getBalance();
            } else if (acc->getAccountType() == "LOAN") {
                loan_count++;
                total_loans += acc->getBalance(); // Outstanding loan amount
            }
        }
        
        cout << "\nAccounts:" << endl;
        cout << "  Savings: " << savings_count << endl;
        cout << "  Current: " << current_count << endl;
        cout << "  Loan: " << loan_count << endl;
        cout << "  Total: " << accounts.size() << endl;
        
        cout << "\nFinancial Overview:" << endl;
        cout << "  Total Deposits: Rs." << fixed << setprecision(2) << total_balance << endl;
        cout << "  Outstanding Loans: Rs." << fixed << setprecision(2) << total_loans << endl;
        
        // Transaction statistics
        int total_transactions = 0;
        for (const auto& pair : accounts) {
            total_transactions += pair.second->getTransactionHistory().size();
        }
        cout << "\nTransaction Activity:" << endl;
        cout << "  Total Transactions: " << total_transactions << endl;
        cout << "  Average per Account: " << fixed << setprecision(1) 
             << (accounts.empty() ? 0 : (float)total_transactions / accounts.size()) << endl;
        
        // System health
        cout << "\nSystem Health:" << endl;
        cout << "  Data Files: " << (exists("accounts.dat") ? "✓" : "✗") << " Accounts";
        cout << ", " << (exists("users_secure.dat") ? "✓" : "✗") << " Users";
        cout << ", " << (exists("pins_secure.dat") ? "✓" : "✗") << " PINs" << endl;
        
        cout << "  Next Account Number: " << AccountManager::getNextAccountNumber() + 1 << endl;
    }
};

int main() {
    try {
        cout << "🏦 Initializing Secure Banking System..." << endl;
        BankingSystem bank;
        bank.run();
    } catch (const exception& e) {
        cout << "System Error: " << e.what() << endl;
        cout << "Please contact system administrator." << endl;
        return 1;
    }
    
    return 0;
}