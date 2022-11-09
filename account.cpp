#include "account.h"

//���ܣ������ֱ�Ϊ�������ױ�ţ���ʱ������棩������ʱ��
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

//ȡ��ܣ������ֱ�Ϊȡ������ױ�ţ���ʱ������棩������ʱ��
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

//ת���տ�ܣ�ת���˻����ã��������ֱ�Ϊת�˽��Է��˺š����ױ�ţ���ʱ������棩������ʱ��
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

//ת�˸���ܣ�ת���˻����ã��������ֱ�Ϊת�˽��Է��˺š����ױ�ţ���ʱ������棩������ʱ��
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
