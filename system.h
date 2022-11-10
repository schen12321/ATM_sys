#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <sys/timeb.h>
#include <ctime>
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include "record.h"
#include "MD5.h"
#include "snack.h"

using std::cout;
using std::cin;
using std::unordered_map;
using std::swap;
using std::ios;
using std::setprecision;
using std::stringstream;
using namespace std;
class System {
    vector<Account> accounts;                                        //��¼�����û�����
    unordered_map<string, Account *> accountIndex;                   //�Կ���Ϊ�������û�����
    Account *currAccount{nullptr};                                   //��ǰ��¼�û���Ϊ�����ʾδ��¼
    string currAccountId;                                            //��ǰ�û����ţ�Ϊ�����ʾδ��¼
    bool isAdmin{false};                                             //�Ƿ�Ϊ����Ա�˺�
    const string adminId{"1"};                              //�������Ա�˻�ID
    const string adminPassword{"1"};                            //�������Ա�˻�����

public:
    //��Ĺ��캯�������ļ��м����˻���Ϣ
    System();

    //����ATMģ������֮����ѭ����ֱ������˳���ťʹ��exit(0)��������
    void start();

private:
    //����ʵ���˻���¼����
    int signIn();

    //����ʵ�ֹ����¼����
    int adminSignIn();

    //����ʵ���˻��˳�����
    int signOut();

    //����ʵ�ֹ���Ա�����˻�����
    int signUp();

    //����ʵ���޸����빦��
    int changePassword();

    //����ʵ���˻�ע�����ܣ�ѡ��
    int deleteAccount();

    //����ʵ�ִ���
    int deposit();

    //����ʵ��ȡ���
    int withdrawal();

    //����ʵ��ת�˹���
    int transfer();

    //����ʵ������ѯ����
    int showBalance();

    //�����ť���Խ��е�¼���˳�����������ֿ��ܷ����Ĵ���
    void signInMenu();

    //�����ť���Խ�����Ӧ�Ĳ�������������ֿ��ܷ����Ĵ���
    void mainMenu();

    //��ȡʱ���
    static string getTimestamp();

    //��ȡ��ʽ���ĵ�ǰʱ�䣨�磺"2021-09-09 22:02:35"��
    //��˵�⼸��ÿ�춼д���뵽ʮһ��࣬Ҫͺ��Ҫͺ��
    static string getCurrentTime();
};
