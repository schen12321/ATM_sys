#include <string>
#include <vector>

using std::string;
using std::vector;

//�������ڴ��浥���û��Ļ�����Ϣ�ͽ��׼�¼���ṩ�����������Ӳ�ͬ���͵Ľ��׼�¼
class Account {
public:
    //������Ϣ
    string id;                                                      //����
    string name;                                                    //����
    string passwd;                                                  //����
    double balance{0};                                              //�˻����
    int wrongPasswdLeft{};                                          //������������������

    //���ʽ��׼�¼
    struct Transaction {
        string transactionId;                                        //���ױ��
        string transactionTime;                                      //����ʱ��
        int transactionType;                                         //�������� 1-��� 2-ȡ�� 3-ת�� 4-ת��
        double transactionAmount;                                    //���׽��
        string counterpartyAccount;                                  //ת�˶Է��˺� ����ȡ����Ϊ"this"
    };

    vector<Transaction> transactionHistory;                          //��ʷ���׼�¼

    //���Ĭ�Ϲ��캯��
    Account() = default;

    //�ṩ���š�������������๹�캯�����������˺�ʱʹ��
    Account(string &id, string &name, string &passwd) : id(id), name(name), passwd(passwd) {
        wrongPasswdLeft = 3;
    }

    //�ṩȫ����Ϣ���๹�캯�����ӱ����ļ���ȡ�˻���Ϣʱʹ��
    Account(string &id, string &name, string &passwd, double &balance, int &wrongPasswdLeft, vector<Transaction> &transactionHistory) :
            id(id), name(name), passwd(passwd), balance(balance), wrongPasswdLeft(wrongPasswdLeft), transactionHistory(transactionHistory) {}

    //����һ�����׼�¼�������ֱ�Ϊ�������ױ�š�����ʱ��
    void deposit(double amount, const string &timestamp, const string &currTime);

    //����һ��ȡ��׼�¼�������ֱ�Ϊȡ������ױ�š�����ʱ��
    void withdrawal(double amount, const string &timestamp, const string &currTime);

    //����һ��ת���տ�׼�¼��ת���˻����ã��������ֱ�Ϊת�˽��Է��˺š����ױ�š�����ʱ��
    void transferIn(double amount, const string &counterpartyAccount, const string &timestamp, const string &currTime);

    //����һ��ת�˸���׼�¼��ת���˻����ã��������ֱ�Ϊת�˽��Է��˺š����ױ�š�����ʱ��
    void transferOut(double amount, const string &counterpartyAccount, const string &timestamp, const string &currTime);

    Account& operator=(const Account& acc){ //��ֵ����
        if(this!=&acc){
            id = acc.id;                                                        //����
            name = acc.name;                                                    //����
            passwd = acc.passwd;                                                //����
            balance = acc.balance;                                              //�˻����
            wrongPasswdLeft = acc.wrongPasswdLeft;
            transactionHistory = acc.transactionHistory;
        }
        return *this;
    }
};
