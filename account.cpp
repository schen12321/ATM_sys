#include "account.h"

//存款功能，参数分别为存款金额、交易编号（用时间戳代替）、交易时间
void Account::deposit(double amount, const string &timestamp, const string &currTime) {
    balance += amount;
    Transaction transaction;
    transaction.transactionId = timestamp;
    transaction.transactionTime = currTime;
    transaction.transactionType = 1;
    transaction.transactionAmount = amount;
    transaction.counterpartyAccount = "this";
    transactionHistory.push_back(transaction);
}

//取款功能，参数分别为取款金额、交易编号（用时间戳代替）、交易时间
void Account::withdrawal(double amount, const string &timestamp, const string &currTime) {
    balance -= amount;
    Transaction transaction;
    transaction.transactionId = timestamp;
    transaction.transactionTime = currTime;
    transaction.transactionType = 2;
    transaction.transactionAmount = amount;
    transaction.counterpartyAccount = "this";
    transactionHistory.push_back(transaction);
}

//转账收款功能（转入账户调用），参数分别为转账金额、对方账号、交易编号（用时间戳代替）、交易时间
void Account::transferIn(double amount, const string &counterpartyAccount, const string &timestamp, const string &currTime) {
    balance += amount;
    Transaction transaction;
    transaction.transactionId = timestamp;
    transaction.transactionTime = currTime;
    transaction.transactionType = 3;
    transaction.transactionAmount = amount;
    transaction.counterpartyAccount = counterpartyAccount;
    transactionHistory.push_back(transaction);
}

//转账付款功能（转出账户调用），参数分别为转账金额、对方账号、交易编号（用时间戳代替）、交易时间
void Account::transferOut(double amount, const string &counterpartyAccount, const string &timestamp, const string &currTime) {
    balance -= amount;
    Transaction transaction;
    transaction.transactionId = timestamp;
    transaction.transactionTime = currTime;
    transaction.transactionType = 4;
    transaction.transactionAmount = amount;
    transaction.counterpartyAccount = counterpartyAccount;
    transactionHistory.push_back(transaction);
}
