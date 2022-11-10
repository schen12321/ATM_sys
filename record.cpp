#include "record.h"
#include <iostream>

using std::cout;

inline void Record::writeString(ofstream &fout, const string &s) {
	auto len = s.size();
    fout.write((const char *) &len, 4);
    fout.write(s.data(), s.size());
}

inline void Record::readString(ifstream &fin, string &s) {
	int len;
    fin.read((char *) &len, 4);

    char c;
    for (int i = 0; i < len; ++i) {

        fin.read(&c, 1);
        s += c;
    }
}


void Record::loadRecord(vector<Account> &accounts) {
	ifstream fin("ATM_sys.dat", ifstream::in | ifstream::binary);

    if (!fin) return;

    int n;                                                        //读取用户数
    fin.read((char *) &n, 4);

    for (int i = 0; i<n; i++){
    	string id;                                                   //读取卡号
        readString(fin, id);

        string name;                                                 //读取姓名
        readString(fin, name);

        string passwd;                                             //读取密码
        readString(fin, passwd);

        double balance;                                              //读取余额
        fin.read((char *) &balance, 8);

        int wrongPasswdLeft;                                       //读取剩余密码输入错误次数
        fin.read((char *) &wrongPasswdLeft, 4);

        int num;                                                     //读取用户交易记录条数
        fin.read((char *) &num, 4);

        vector<Account::Transaction> transactionHistory;             //用于保存读取的交易记录
        for (int j = 0; j < num; ++j) {

            string transactionId;                                    //读取交易编号
            readString(fin, transactionId);

            string transactionTime;                                  //读取交易时间
            readString(fin, transactionTime);

            int transactionType;                                     //读取交易类型（存款取款或转账）
            fin.read((char *) &transactionType, 4);

            double transactionAmount;                                //读取交易金额
            fin.read((char *) &transactionAmount, 8);

            string counterpartyAccount;                              //读取交易对方账号（若为类型为存取款则对方账号为"this"）
            readString(fin, counterpartyAccount);

            transactionHistory.push_back({transactionId, transactionTime, transactionType, transactionAmount, counterpartyAccount});
        }

        Account account(id, name, passwd, balance, wrongPasswdLeft, transactionHistory);
        accounts.push_back(account);

    }
    fin.close();
}

void Record::saveRecord(const vector<Account> &accounts) {
    ofstream fout("ATM_sys.dat", ofstream::out | ofstream::trunc | ofstream::binary);
    if (!fout) return;
    auto n = accounts.size();                                        //写入用户数
    fout.write((const char *) &n, 4);

    for (const auto &account: accounts) {

        writeString(fout, account.id);                               //写入卡号

        writeString(fout, account.name);                             //写入姓名

        writeString(fout, account.passwd);                         //写入密码

        fout.write((const char *) &account.balance, 8);              //写入余额

        fout.write((const char *) &account.wrongPasswdLeft, 4);    //写入剩余密码输入错误次数

        auto num = account.transactionHistory.size();                //写入用户交易记录条数
        fout.write((const char *) &num, 4);

        for (const auto &transaction: account.transactionHistory) {

            writeString(fout, transaction.transactionId);            //写入交易编号

            writeString(fout, transaction.transactionTime);          //写入交易时间

            fout.write((const char *) &transaction.transactionType, 4);   //写入交易类型

            fout.write((const char *) &transaction.transactionAmount, 8); //写入交易金额

            writeString(fout, transaction.counterpartyAccount);           //写入交易对方账号
        }
    }
    fout.close();
}

void Record::printVoucher(const Account::Transaction &transaction) {
    cout << "交易编号：" << transaction.transactionId << endl;
    cout << "交易时间：" << transaction.transactionTime << endl;
    cout << "交易类型：";
    if (transaction.transactionType == 1) {
        cout << "存款" << endl;
        cout << "存款金额：CNY " << transaction.transactionAmount << endl;
    } else if (transaction.transactionType == 2) {
        cout << "取款" << endl;
        cout << "取款金额：CNY " << transaction.transactionAmount << endl;
    } else if (transaction.transactionType == 3) {
        cout << "转账收款" << endl;
        cout << "收款金额：CNY " << transaction.transactionAmount << endl;
        cout << "对方账号：" << transaction.counterpartyAccount << endl;
    } else if (transaction.transactionType == 4) {
        cout << "转账付款" << endl;
        cout << "付款金额：CNY " << transaction.transactionAmount << endl;
        cout << "对方账号：" << transaction.counterpartyAccount << endl;
    }
}

void Record::exportTransactionHistory(const vector<Account::Transaction> &transactionHistory) {
    for (const auto &transaction: transactionHistory) {
        cout << "交易编号：" << transaction.transactionId << endl;
        cout << "交易时间：" << transaction.transactionTime << endl;
        cout << "交易类型：";
        if (transaction.transactionType == 1) {
            cout << "存款" << endl;
            cout << "存款金额：CNY " << transaction.transactionAmount << endl;
        } else if (transaction.transactionType == 2) {
            cout << "取款" << endl;
            cout << "取款金额：CNY " << transaction.transactionAmount << endl;
        } else if (transaction.transactionType == 3) {
            cout << "转账收款" << endl;
            cout << "收款金额：CNY " << transaction.transactionAmount << endl;
            cout << "对方账号：" << transaction.counterpartyAccount << endl;
        } else if (transaction.transactionType == 4) {
            cout << "转账付款" << endl;
            cout << "付款金额：CNY " << transaction.transactionAmount << endl;
            cout << "对方账号：" << transaction.counterpartyAccount << endl;
        }
        cout << endl;
    }
}