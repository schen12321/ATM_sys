#include "system.h"


System::System() {
    Record::loadRecord(accounts);
    for (auto &account: accounts)
        accountIndex[account.id] = &account;
}

//����ATM��֮������ѭ����ֱ������˳���ťʹ��exit(0)��������
void System::start() {
        mainMenu();
}

int System::signIn() {
    string id, id_bak, passwd;
    cout << "�����뿨��:";
    cin >> id;
    id_bak = id;
    id = MD5(id).toStr();
    if (!accountIndex.count(id)) {
        cout << "�˻�������" << endl;
        return -1;
    }
    if (!accountIndex[id]->wrongPasswdLeft) {
        cout << "�˻��Ѷ���" << endl;
        return -2;
    }
    cout << "����������:";
    cin >> passwd;
    passwd = MD5(passwd).toStr();
    cout << accountIndex[id]->wrongPasswdLeft;
    if (accountIndex[id]->passwd != passwd) {
        cout << "�������";
        accountIndex[id]->wrongPasswdLeft -= 1;
        Record::saveRecord(accounts);
        switch (accountIndex[id]->wrongPasswdLeft) {
            case 2:
                cout << " , ����2�λ���" << endl;
                return -3;
            case 1:
                cout << ", ����1�λ���" << endl;
                return -4;
            case 0:
                cout << ", �˻��Ѷ���, ����ϵ����Ա�ⶳ" << endl;
                return -5;
        }
    }
    cout << "��½�ɹ�" << endl;
    currAccount = accountIndex[id];
    currAccountId = id_bak;
    currAccount->wrongPasswdLeft = 3;        //����ʣ��������
    return 1;
}

int System::adminSignIn() {
    string id, id_bak, passwd;
    cout << "���������Ա�˻�ID:";
    cin >> id;
    id_bak = id;
    if (id == adminId) {
        cout << "���������Ա�˻�����:";
        cin >> passwd;
        if (passwd != adminPassword) {
            cout << "�������" << endl;
            return -1;
        }
        cout << "����Ա��½�ɹ�" << endl;
        isAdmin = true;
        return 1;
    }
    cout << "�ǹ���ԱID" << endl;
    return -2;
}

int System::signOut() {
    if (currAccount) {
        currAccount = nullptr;
        currAccountId.clear();
        cout << "�˻��˳��ɹ�" << endl;
        return 1;
    } else if (isAdmin) {
        isAdmin = false;
        cout << "����Ա�˻��˳��ɹ�" << endl;
        return 2;
    }
    cout << "Error: �˻�δ��¼" << endl;
    return -1;
}

int System::signUp() {
    //���뿨����������
    string id, name, passwd;
    cout << "�����뿨��:";
    cin >> id;
    id = MD5(id).toStr();
    if (accountIndex.count(id)) {
        cout << "�˿����Ѵ��ڣ�" << endl;
        return 0;
    }
    cout << "�������������:";
    cin >> name;
    cout << "����������:";
    cin >> passwd;
    passwd = MD5(passwd).toStr();

    //�����û��ɹ�
    Account account(id, name, passwd);
    accounts.push_back(account);
    for (auto &acc: accounts) {
        accountIndex[acc.id] = &(account);
    }
    Record::saveRecord(accounts);
    cout << "�û����ӳɹ�!" << endl;
    return 1;
}

//�޸�����
int System::changePassword() {
    //����Ա��������
    if (isAdmin) {
        string id, newPasswd;
        char ch;
        cout << "������Ҫ�޸ĵĿ���:";
        cin >> id;
        id = MD5(id).toStr();
        if (!accountIndex.count(id)) {
            cout << "���Ų����ڣ�" << endl;
            return 0;
        }
        cout << "������������:";
        cin >> newPasswd;
        newPasswd = MD5(newPasswd).toStr();
        if (newPasswd == accountIndex[id]->passwd) {
            cout << "�¾�������ͬ!" <<endl;
            return 0;
        }
        cout << "��ȷ���Ƿ��޸�<Y/N>: ";
        cin >> ch;
        if (ch != 'y' && ch != 'Y') {
            cout << "����ȡ��" << endl;
            return 0;
        }
        accountIndex[id]->wrongPasswdLeft = 3;
        accountIndex[id]->passwd = newPasswd;
    }
        //�ǹ���Ա
    else {
        string newPasswd, passwdConfirm;
        char ch;
        cout << "������������: ";
        cin >> newPasswd;
        cout << "���ٴ�ȷ��������: ";
        cin >> passwdConfirm;
        if (passwdConfirm != newPasswd) {
            cout << "������������벻��ͬ!" << endl;
            return 0;
        }
        newPasswd = MD5(newPasswd).toStr();
        if (newPasswd == currAccount->passwd) {
            cout << "���������������ͬ!" << endl;
            return 0;
        }
        cout << "��ȷ���Ƿ��޸�<Y/N>: ";
        cin >> ch;
        if (ch != 'y' && ch != 'Y') {
            cout << "����ȡ��" << endl;
            return 0;
        }
        currAccount->passwd = newPasswd;
    }
    Record::saveRecord(accounts);//����
    cout << "�����޸ĳɹ�!" << endl;
    return 1;
}

//�˻�ע��
int System::deleteAccount() {
    if (isAdmin) {
        string id;
        char ch;
        cout << "������Ҫע�����˻�id: ";
        cin >> id;
        id = MD5(id).toStr();
        if (!accountIndex.count(id)) {
            cout << "�˻�������" << endl;
            return -1;
        }
        cout << "��ȷ���Ƿ�Ҫע�����˻�<Y/N>: ";
        cin >> ch;
        if (ch != 'y' && ch != 'Y') {
            cout << "����ȡ��" << endl;
            return -2;
        }

        /*for (auto &account: accounts) {
            accountIndex[account.id] = &(account);
        }*/
        swap(*accountIndex[id], *(accounts.end() - 1));
        accounts.pop_back();
        accountIndex.erase(id);
        Record::saveRecord(accounts);
        cout << "�˻�ע���ɹ�" << endl;
        return 1;
    }
    else {
        string passwd;
        char ch;
        cout << "����������: ";
        cin >> passwd;
        passwd = MD5(passwd).toStr();
        if (passwd != currAccount->passwd){
            cout << "�������";
            return -1;
        }
        cout << "�������Ϊ " <<setiosflags(ios::fixed) << setprecision(2)<< currAccount->balance << " Ԫ,�Ƿ�ȷ��ע��<Y/N>";
        cin >> ch;
        if (ch != 'y' && ch != 'Y'){
            cout << "����ȡ��";
            return -2;
        }
        //�ɹ�ע��
        /*for (auto &account: accounts) {
            accountIndex[account.id] = &(account);
        }*/
        accountIndex.erase(currAccount->id);
        swap(*currAccount, *(accounts.end() - 1));
        accounts.pop_back();
        currAccount = nullptr;
        currAccountId.clear();
        Record::saveRecord(accounts);
        cout << "�˻�ע���ɹ�" << endl;
        return 1;
    }
}

int System::showBalance() {
    if (isAdmin) {
        string id;
        cout << "��������Ҫ�鿴�Ŀ���: ";
        cin >> id;
        id = MD5(id).toStr();
        if (!accountIndex.count(id)) {
            cout << "���Ų�����" << endl;
            return 0;
        }
        cout << "���˺ŵ�ǰ�����Ϊ " << setiosflags(ios::fixed) << setprecision(2) << accountIndex[id]->balance
             << " Ԫ";
    }
    else {
        cout << "����ǰ�����Ϊ " << setiosflags(ios::fixed) << setprecision(2) << currAccount->balance << " Ԫ";
    }
    return 1;
}

int System::deposit() {
    if (!currAccount) {
        cout << "Error: �˻�δ��¼" << endl;
        return -1;
    }
    string deposit_amount_str;
    cout << "����������: " << endl;
    cin >> deposit_amount_str;
    double deposit_amount;
    try {
        deposit_amount = stod(deposit_amount_str);
    }
    catch (...) {
        cout << "�������" << endl;
        return -1;
    }
    if (deposit_amount <= 0) {
        cout << "������Ϊ0" << endl;
        return -2;
    }
    if ((int) (deposit_amount / 100) * 100 != deposit_amount) {
        cout << "��������Ϊ100��������" << endl;
        return -3;
    }
    cout << "��ȷ�� �����Ϊ " << setiosflags(ios::fixed) << setprecision(2) << deposit_amount << " Ԫ<Y>" << endl;
    char ch;
    cin >> ch;
    if (ch != 'y' && ch != 'Y')
        cout << "ȡ������" << endl;
    currAccount->deposit(deposit_amount, getTimestamp(), getCurrentTime());
    cout << "�ɹ���� " << deposit_amount << " Ԫ" << endl;
    cout << "�������Ϊ " << setiosflags(ios::fixed) << setprecision(2) << currAccount->balance << " Ԫ" << endl;
    cout << "�Ƿ��ӡƾ֤<Y/N>" << endl;
    cin >> ch;
    if (ch == 'y' || ch == 'Y')
        Record::printVoucher(currAccount->transactionHistory.back());
    Record::saveRecord(accounts);
    return 1;
}

int System::withdrawal() {
    if (!currAccount) {
        cout << "Error: �˻�δ��¼" << endl;
        return -1;
    }
    string withdrawal_amount_str;
    cout << "������ȡ����: " << endl;
    cin >> withdrawal_amount_str;
    double withdrawal_amount;
    try {
        withdrawal_amount = stod(withdrawal_amount_str);
    }
    catch (...) {
        cout << "�������" << endl;
        return -1;
    }
    if (withdrawal_amount <= 0) {
        cout << "ȡ�����Ϊ0" << endl;
        return -2;
    }
    if ((int) (withdrawal_amount / 100) * 100 != withdrawal_amount) {
        cout << "ȡ�������Ϊ100��������" << endl;
        return -3;
    }
    if (withdrawal_amount > currAccount->balance) {
        cout << "���㣬�������Ϊ " << setiosflags(ios::fixed) << setprecision(2) << currAccount->balance << " Ԫ"
             << endl;
        return -4;
    }
    cout << "��ȷ�� ȡ����Ϊ " << setiosflags(ios::fixed) << setprecision(2) << withdrawal_amount << " Ԫ<Y>" << endl;
    char ch;
    cin >> ch;
    if (ch != 'y' && ch != 'Y') {
        cout << "ȡ������" << endl;
        return -5;
    }
    currAccount->withdrawal(withdrawal_amount, getTimestamp(), getCurrentTime());
    cout << "�ɹ�ȡ�� " << withdrawal_amount << " Ԫ" << endl;
    cout << "�������Ϊ " << setiosflags(ios::fixed) << setprecision(2) << currAccount->balance << " Ԫ" << endl;
    cout << "���պ�����ȡ��" << endl;
    cout << "�Ƿ��ӡƾ֤<Y/N>" << endl;
    cin >> ch;
    if (ch == 'y' || ch == 'Y')
        Record::printVoucher(currAccount->transactionHistory.back());
    Record::saveRecord(accounts);
    return 1;
}

int System::transfer() {
    if (!currAccount) {
        cout << "Error: �˻�δ��¼" << endl;
        return -1;
    }
    string toId, toId_copy;
    cout << "������Է��˻�:";
    cin >> toId;
    toId_copy = toId;
    toId = MD5(toId).toStr();
    if (!accountIndex.count(toId)) {
        cout << "�Է��˻�������" << endl;
        return -1;
    }
    if (toId_copy == currAccountId) {
        cout << "ת�˶�����Ϊ����" << endl;
    }
    string transfer_amount_str;
    double transfer_amount;
    cout << "������ת�˽��:";
    cin >> transfer_amount_str;
    try {
        transfer_amount = stod(transfer_amount_str);
    }
    catch (...) {
        cout << "�������" << endl;
        return -2;
    }
    if (transfer_amount <= 0) {
        cout << "ת�˽���Ϊ0" << endl;
        return -3;
    }
    if (transfer_amount > currAccount->balance) {
        cout << "���㣬�������Ϊ " << setiosflags(ios::fixed) << setprecision(2) << currAccount->balance << " Ԫ"
             << endl;
        return -4;
    }
    cout << "��ת�˸� " << toId_copy << " " << setiosflags(ios::fixed) << setprecision(2) << transfer_amount << " Ԫ<Y>"
         << endl;
    char ch;
    cin >> ch;
    if (ch != 'y' && ch != 'Y') {
        cout << "ȡ������" << endl;
        return -5;
    }

    auto timestamp = getTimestamp();
    auto currTime = getCurrentTime();
    currAccount->transferOut(stod(transfer_amount_str), toId_copy, timestamp, currTime);
    accountIndex[toId]->transferIn(stod(transfer_amount_str), currAccountId, timestamp, currTime);
    cout << "�ɹ�ת�˸� " << toId_copy << " " << setiosflags(ios::fixed) << setprecision(2) << transfer_amount << " Ԫ"
         << endl;
    cout << "�������Ϊ " << setiosflags(ios::fixed) << setprecision(2) << currAccount->balance << " Ԫ" << endl;
    cout << "�Ƿ��ӡƾ֤<Y/N>" << endl;
    cin >> ch;
    if (ch == 'y' || ch == 'Y')
        Record::printVoucher(currAccount->transactionHistory.back());
    Record::saveRecord(accounts);
    return 1;
}

void System::mainMenu() {
    int choice1, choice2, choice3, choice4;
    do {
        cout << "        ��------------------------��\n";
        cout << "            #��ӭʹ��ATM�������#\n\n";
        cout << "              ��1�� ����Ա��¼     \n";
        cout << "              ��2�� �û���¼     \n";
        cout << "              ��0�� �˳����     \n\n";
        cout << "        ��------------------------��\n";
        cin >> choice1;
        switch (choice1) {
            case 1:
                adminSignIn();
                if (!isAdmin) {
                    continue;
                }
                do {
                    cout << "        ��------------------------��\n";
                    cout << "                 #�˻�����#\n\n";
                    cout << "              ��1�� ����\n";
                    cout << "              ��2�� ����\n";
                    cout << "              ��3�� �޸�����\n";
                    cout << "              ��0�� �˳���¼\n\n";
                    cout << "        ��------------------------��\n";
                    cin >> choice2;
                    switch (choice2) {
                        case 1:
                            cout << "        ��------------------------��\n";
                            cout << "                   #����#\n";
                            cout << "        ��------------------------��\n";
                            cout << "              #¼���˻���Ϣ#\n";
                            signUp();
                            break;
                        case 2:
                            cout << "        ��------------------------��\n";
                            cout << "                  #����#\n\n";
                            deleteAccount();
                            break;
                        case 3:
                            cout << "        ��------------------------��\n";
                            cout << "                 #�޸�����#\n\n";
                            changePassword();
                            break;
                        case 0:
                            signOut();
                            break;
                        default:
                            cout << "�����������������!\n";
                    }
                } while (choice2 != 0);
                break;
            case 2:
                signIn();
                if (!currAccount) {
                    continue;
                }
                do {
                    cout << "        ��------------------------��\n";
                    cout << "              #��ӭ" << currAccount->name << "#\n\n";
                    cout << "              ��1�� ���\n";
                    cout << "              ��2�� ȡ��\n";
                    cout << "              ��3�� �˻���ѯ\n";
                    cout << "              ��4�� ת��\n";
                    cout << "              ��5�� �޸�����\n";
                    cout << "              ��6�� ����\n";
                    cout << "              ��0�� �˳�\n\n";
                    cout << "        ��------------------------��\n";
                    cin >> choice3;
                    switch (choice3) {
                        case 1:
                            cout << "        ��------------------------��\n";
                            cout << "                  #���#\n\n";
                            deposit();
                            break;
                        case 2:
                            cout << "        ��------------------------��\n";
                            cout << "                 #ȡ��#\n\n";
                            withdrawal();
                            break;
                        case 3:
                            do {
                                cout << "        ��------------------------��\n";
                                cout << "                  #��ѯ#\n\n";
                                cout << "               ��1�� ���\n";
                                cout << "               ��2�� ���׼�¼\n";
                                cout << "               ��0�� ����\n\n";
                                cout << "        ��------------------------��\n";
                                cin >> choice4;
                                switch (choice4) {
                                    case 1:
                                        cout << "        ��------------------------��\n";
                                        cout << "                 #���#\n\n";
                                        showBalance();
                                        break;
                                    case 2:
                                        cout << "        ��------------------------��\n";
                                        cout << "                 #���׼�¼#\n\n";
                                        Record::exportTransactionHistory(currAccount->transactionHistory);
                                        break;
                                    case 0:
                                        break;
                                    default:
                                        cout << "�����������������!\n";
                                }
                            } while (choice4 != 0);
                            break;
                        case 4:
                            cout << "        ��------------------------��\n";
                            cout << "                 #ת��#\n\n";
                            transfer();
                            break;
                        case 5:
                            cout << "        ��------------------------��\n";
                            cout << "                #�޸�����#\n\n";
                            changePassword();
                            break;
                        case 6:
                            cout << "        ��------------------------��\n";
                            cout << "                  #����#\n\n";
                            deleteAccount();
                            break;
                        case 0:
                            signOut();
                            break;
                        default:
                            cout << "�����������������!\n";
                    }
                } while (choice3 != 0);
                break;
            case 0:
                printf("��л����ʹ��!\n");
                break;
            default:
                printf("�����������������!\n");
        }
    } while (choice1 != 0);
    exit(0);
}

string System::getTimestamp() {
    timeb timestamp{};
    ftime(&timestamp);

    //���������ܲ�����λ��Ҫ����0
    stringstream ss;
    ss << setw(3) << setfill('0') << timestamp.millitm;

    return to_string(timestamp.time) + ss.str();
}

//��ȡ��ʽ���ĵ�ǰʱ�䣨�磺"2021-09-09 22:02:35"��
string System::getCurrentTime() {
    //�������������ڻ�ȡ��ǰʱ��Ӵ
    tm currTime{};
    time_t timestamp = time(nullptr);

    localtime_s(&currTime, &timestamp);

    //���ڴ������ո�ʽ���Ľ��
    string ans;
    //���ڸ�ʽ����ǰʱ��
    stringstream ss;

    //��
    ans += to_string(1900 + currTime.tm_year) + "-";

    //��
    ss << setw(2) << setfill('0') << 1 + currTime.tm_mon;
    ans += ss.str() + "-";

    //��
    ss.str("");
    ss << setw(2) << setfill('0') << currTime.tm_mday;
    ans += ss.str() + " ";

    //ʱ
    ss.str("");
    ss << setw(2) << setfill('0') << currTime.tm_hour;
    ans += ss.str() + ":";

    //��
    ss.str("");
    ss << setw(2) << setfill('0') << currTime.tm_min;
    ans += ss.str() + ":";

    //��
    ss.str("");
    ss << setw(2) << setfill('0') << currTime.tm_sec;
    ans += ss.str();
    return ans;
}