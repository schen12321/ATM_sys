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

    int n;                                                        //��ȡ�û���
    fin.read((char *) &n, 4);

    for (int i = 0; i<n; i++){
    	string id;                                                   //��ȡ����
        readString(fin, id);

        string name;                                                 //��ȡ����
        readString(fin, name);

        string passwd;                                             //��ȡ����
        readString(fin, passwd);

        double balance;                                              //��ȡ���
        fin.read((char *) &balance, 8);

        int wrongPasswdLeft;                                       //��ȡʣ����������������
        fin.read((char *) &wrongPasswdLeft, 4);

        int num;                                                     //��ȡ�û����׼�¼����
        fin.read((char *) &num, 4);

        vector<Account::Transaction> transactionHistory;             //���ڱ����ȡ�Ľ��׼�¼
        for (int j = 0; j < num; ++j) {

            string transactionId;                                    //��ȡ���ױ��
            readString(fin, transactionId);

            string transactionTime;                                  //��ȡ����ʱ��
            readString(fin, transactionTime);

            int transactionType;                                     //��ȡ�������ͣ����ȡ���ת�ˣ�
            fin.read((char *) &transactionType, 4);

            double transactionAmount;                                //��ȡ���׽��
            fin.read((char *) &transactionAmount, 8);

            string counterpartyAccount;                              //��ȡ���׶Է��˺ţ���Ϊ����Ϊ��ȡ����Է��˺�Ϊ"this"��
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
    auto n = accounts.size();                                        //д���û���
    fout.write((const char *) &n, 4);

    for (const auto &account: accounts) {

        writeString(fout, account.id);                               //д�뿨��

        writeString(fout, account.name);                             //д������

        writeString(fout, account.passwd);                         //д������

        fout.write((const char *) &account.balance, 8);              //д�����

        fout.write((const char *) &account.wrongPasswdLeft, 4);    //д��ʣ����������������

        auto num = account.transactionHistory.size();                //д���û����׼�¼����
        fout.write((const char *) &num, 4);

        for (const auto &transaction: account.transactionHistory) {

            writeString(fout, transaction.transactionId);            //д�뽻�ױ��

            writeString(fout, transaction.transactionTime);          //д�뽻��ʱ��

            fout.write((const char *) &transaction.transactionType, 4);   //д�뽻������

            fout.write((const char *) &transaction.transactionAmount, 8); //д�뽻�׽��

            writeString(fout, transaction.counterpartyAccount);           //д�뽻�׶Է��˺�
        }
    }
    fout.close();
}

void Record::printVoucher(const Account::Transaction &transaction) {
    cout << "���ױ�ţ�" << transaction.transactionId << endl;
    cout << "����ʱ�䣺" << transaction.transactionTime << endl;
    cout << "�������ͣ�";
    if (transaction.transactionType == 1) {
        cout << "���" << endl;
        cout << "����CNY " << transaction.transactionAmount << endl;
    } else if (transaction.transactionType == 2) {
        cout << "ȡ��" << endl;
        cout << "ȡ���CNY " << transaction.transactionAmount << endl;
    } else if (transaction.transactionType == 3) {
        cout << "ת���տ�" << endl;
        cout << "�տ��CNY " << transaction.transactionAmount << endl;
        cout << "�Է��˺ţ�" << transaction.counterpartyAccount << endl;
    } else if (transaction.transactionType == 4) {
        cout << "ת�˸���" << endl;
        cout << "�����CNY " << transaction.transactionAmount << endl;
        cout << "�Է��˺ţ�" << transaction.counterpartyAccount << endl;
    }
}

void Record::exportTransactionHistory(const vector<Account::Transaction> &transactionHistory) {
    for (const auto &transaction: transactionHistory) {
        cout << "���ױ�ţ�" << transaction.transactionId << endl;
        cout << "����ʱ�䣺" << transaction.transactionTime << endl;
        cout << "�������ͣ�";
        if (transaction.transactionType == 1) {
            cout << "���" << endl;
            cout << "����CNY " << transaction.transactionAmount << endl;
        } else if (transaction.transactionType == 2) {
            cout << "ȡ��" << endl;
            cout << "ȡ���CNY " << transaction.transactionAmount << endl;
        } else if (transaction.transactionType == 3) {
            cout << "ת���տ�" << endl;
            cout << "�տ��CNY " << transaction.transactionAmount << endl;
            cout << "�Է��˺ţ�" << transaction.counterpartyAccount << endl;
        } else if (transaction.transactionType == 4) {
            cout << "ת�˸���" << endl;
            cout << "�����CNY " << transaction.transactionAmount << endl;
            cout << "�Է��˺ţ�" << transaction.counterpartyAccount << endl;
        }
        cout << endl;
    }
}