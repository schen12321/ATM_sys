#include <fstream>
#include <vector>
#include "account.h"

using std::ifstream;
using std::ofstream;
using std::vector;
using std::endl;

//���ฺ�����ļ������������ز���
class Record {

    //C++��string���͵��ַ������ֱ��ʹ�ó����������͵ķ�ʽ���ж������ļ�������ܻ�����һЩ����
    //����ʱ���ȶ�ȡ�ַ�������len���ٶ�ȡlen��char�����ַ�������ƴ�ӳ�string�����ַ���

    //���ַ����Զ�������ʽд���ļ�������Ϊ�ļ��������Ҫд���ļ����ַ���
    static inline void writeString(ofstream &, const string &);

    //��ȡ�Զ�������ʽд���ļ����ַ���������Ϊ�ļ��������ͽ��ն�ȡ���ݵ��ַ���������
    static inline void readString(ifstream &, string &);

public:
    //���ļ���ȡ���ݣ�����Ϊ�����������ݵ��˺�����������
    static void loadRecord(vector<Account> &accounts);

    //������д���ļ�������Ϊ�洢�������ݵ��˺������ĳ�������
    static void saveRecord(const vector<Account> &accounts);

    //���ƾ�ݣ�����ΪҪ��ӡ�ĵ������׼�¼
    static void printVoucher(const Account::Transaction &transaction);

    //���������ʷ��¼������ΪҪ���������н��׼�¼
    static void exportTransactionHistory(const vector<Account::Transaction> &transactionHistory);
};

