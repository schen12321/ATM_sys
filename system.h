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
#include <conio.h>
#include <windows.h>

using namespace std;

class System {
private:
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

    //����ʵ�ֲ˵�����ʾ
    void mainMenu();

    //��ȡʱ���
    static string getTimestamp();

    //��ȡ��ʽ���ĵ�ǰʱ�䣨�磺"2021-09-09 22:02:35"��
    static string getCurrentTime();
};
