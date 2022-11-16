#include "system.h"

void goToScreen(int xx, int yy) {
    COORD scrn;
    HANDLE hOuput = GetStdHandle(STD_OUTPUT_HANDLE);
    scrn.X = xx;
    scrn.Y = yy;
    SetConsoleCursorPosition(hOuput, scrn);
}

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
    cout << "���������Ŀ���:";
    cin >> id;
    id_bak = id;
    id = MD5(id).toStr();
    if (!accountIndex.count(id)) {
        cout << "���Ų�����" << endl;
        return -1;
    }
    if (!accountIndex[id]->wrongPasswdLeft) {
        cout << "�����˻��ѱ�����" << endl;
        return -2;
    }
    cout << "��������������:";
    cin >> passwd;
    passwd = MD5(passwd).toStr();                               //������md5��ʽ�洢
    if (accountIndex[id]->passwd != passwd) {
        cout << "�������";
        accountIndex[id]->wrongPasswdLeft -= 1;
        Record::saveRecord(accounts);
        switch (accountIndex[id]->wrongPasswdLeft) {                    //���3���ж϶����˻�
            case 2:
                cout << ", ����2�λ���" << endl;
                return -3;
            case 1:
                cout << ", ����1�λ���" << endl;
                return -4;
            case 0:
                cout << ", �����˻��ѱ�����, ����ϵ����Ա�ⶳ" << endl;
                return -5;
        }
    }
    cout << "��½�ɹ�" << endl;
    currAccount = accountIndex[id];
    currAccountId = id_bak;
    currAccount->wrongPasswdLeft = 3;                                   //����ʣ��������
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
        cout << "�����˻����˳�" << endl;
        return 1;
    } else if (isAdmin) {
        isAdmin = false;
        cout << "����Ա�˻����˳�" << endl;
        return 2;
    }
    cout << "Error: �˻�δ��¼" << endl;
    return -1;
}

int System::signUp() {
    string id, name, passwd;
    cout << "�����뿨��:";                               //���뿨����������
    cin >> id;
    if(id.length() != 6){                              //����λ������ʽ
        cout << "���ű���Ϊ6λ����" << endl;
        return -3;
    }
    for(int i =0 ; i < id.length() ; i++){
        if(id[i] < 48 || id[i] > 57){
            cout << "���ű���Ϊ6λ����" << endl;
            return -4;
        }
    }
    id = MD5(id).toStr();
    if (accountIndex.count(id)) {
        cout << "�˿����Ѵ���" << endl;
        return 0;
    }
    cout << "�����뿪��������:";
    cin >> name;
    cout << "����������(6λ����):";
    cin >> passwd;
    if (passwd.length() != 6) {                         //����λ������ʽ
        cout << "�������Ϊ6λ����" << endl;
        return -1;
    }
    for (int i = 0; i < 6; i++) {
        if (passwd[i] < 48 || passwd[i] > 57) {
            cout << "�������Ϊ6λ����" << endl;
            return -2;
        }
    }
    passwd = MD5(passwd).toStr();
    Account account(id, name, passwd);        //�����û��ɹ�
    accounts.push_back(account);
    for (auto &acc: accounts) {
        accountIndex[acc.id] = &(acc);
    }
    Record::saveRecord(accounts);
    cout << "�����ɹ�" << endl;
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
            cout << "���Ų�����" << endl;
            return 0;
        }
        cout << "������������(6λ����):";
        cin >> newPasswd;
        if (newPasswd.length() != 6) {                          //����λ������ʽ
            cout << "�������Ϊ6λ����" << endl;
            return -1;
        }
        for (int i = 0; i < 6; i++) {
            if (newPasswd[i] < 48 || newPasswd[i] > 57) {
                cout << "�������Ϊ6λ����" << endl;
                return -2;
            }
        }
        newPasswd = MD5(newPasswd).toStr();
        if (newPasswd == accountIndex[id]->passwd) {            //�ж��¾������Ƿ���ͬ
            cout << "�¾�������ͬ" << endl;
            return 0;
        }
        cout << "��ȷ���Ƿ��޸�<Y/N>:";
        cin >> ch;
        if (ch != 'y' && ch != 'Y') {
            cout << "����ȡ��" << endl;
            return 0;
        }
        accountIndex[id]->wrongPasswdLeft = 3;
        accountIndex[id]->passwd = newPasswd;
    }
    //�ǹ���Ա��������
    else {
        string newPasswd, passwdConfirm;
        char ch;
        cout << "������������(6λ����):";
        cin >> newPasswd;
        if (newPasswd.length() != 6) {
            cout << "�������Ϊ6λ����" << endl;
            return -1;
        }
        for (int i = 0; i < 6; i++) {
            if (newPasswd[i] < 48 || newPasswd[i] > 57) {
                cout << "�������Ϊ6λ����" << endl;
                return -2;
            }
        }
        cout << "���ٴ�ȷ��������:";
        cin >> passwdConfirm;
        if (passwdConfirm != newPasswd) {
            cout << "������������벻��ͬ" << endl;
            return 0;
        }
        newPasswd = MD5(newPasswd).toStr();
        if (newPasswd == currAccount->passwd) {
            cout << "�¾�������ͬ" << endl;
            return 0;
        }
        cout << "��ȷ���Ƿ��޸�<Y/N>:";
        cin >> ch;
        if (ch != 'y' && ch != 'Y') {
            cout << "����ȡ��" << endl;
            return 0;
        }
        currAccount->passwd = newPasswd;
    }
    Record::saveRecord(accounts);                               //�����¼
    cout << "�����޸ĳɹ�" << endl;
    return 1;
}

//�˻�ע��
int System::deleteAccount() {
    if (isAdmin) {
        string id;
        char ch;
        cout << "������Ҫע���Ŀ���:";
        cin >> id;
        id = MD5(id).toStr();
        if (!accountIndex.count(id)) {
            cout << "���Ų�����" << endl;
            return -1;
        }
        cout << "��ȷ���Ƿ�ע��<Y/N>:";
        cin >> ch;
        if (ch != 'y' && ch != 'Y') {                               //ȷ�ϲ���
            cout << "����ȡ��" << endl;
            return -2;
        }
        swap(*accountIndex[id], *(accounts.end() - 1));
        accounts.pop_back();
        accountIndex.erase(id);
        for (auto &acc: accounts) {
            accountIndex[acc.id] = &(acc);
        }
        Record::saveRecord(accounts);
        cout << "�˻�ע���ɹ�" << endl;
        return 1;
    } else {
        string passwd;
        char ch;
        cout << "����������:";
        cin >> passwd;
        passwd = MD5(passwd).toStr();
        if (passwd != currAccount->passwd) {
            cout << "�������";
            return -1;
        }
        cout << "�������Ϊ " << setiosflags(ios::fixed) << setprecision(2) << currAccount->balance
             << " Ԫ, ��ȷ���Ƿ�ע��<Y/N>:";                            //��ʾ�����ȷ���Ƿ�ע��
        cin >> ch;
        if (ch != 'y' && ch != 'Y') {
            cout << "����ȡ��";
            return -2;
        }
        accountIndex.erase(currAccount->id);                      //ע���ɹ�
        swap(*currAccount, *(accounts.end() - 1));
        accounts.pop_back();
        currAccount = nullptr;                                       //��ǰ�û��˳�p
        currAccountId.clear();
        Record::saveRecord(accounts);
        cout << "�˻�ע���ɹ�" << endl;
        return 1;
    }
}

int System::showBalance() {
    cout << "����ǰ�����Ϊ " << setiosflags(ios::fixed) << setprecision(2) << currAccount->balance << " Ԫ" << endl;
    return 1;
}

//returnֵΪ��ͬ���͵Ĵ���Qt����
int System::deposit() {
    if (!currAccount) {
        cout << "Error: �˻�δ��¼" << endl;
        return -1;
    }
    string deposit_amount_str;
    cout << "����������:";
    cin >> deposit_amount_str;
    double deposit_amount;
    try {
        deposit_amount = stod(deposit_amount_str);
    }
    catch (...) {                                                       //�������������ַ����µĴ���
        cout << "�������" << endl;
        return -1;
    }
    if (deposit_amount_str.length() >= 10) {                            //��ֹ�������µ����
        cout << "�������" << endl;
        return -2;
    }
    if (deposit_amount <= 0) {                                          //������Ϊ����
        cout << "�������" << endl;
        return -2;
    }
    if (deposit_amount > 10000) {                                       //�����޶�
        cout << "���ʴ��ó��� 10000 Ԫ" << endl;
        return -2;
    }
    if ((int) (deposit_amount / 100) * 100 != deposit_amount) {         //����Ϊ100��������
        cout << "��������Ϊ100��������" << endl;
        return -3;
    }
    cout << "��ȷ�� �����Ϊ " << setiosflags(ios::fixed) << setprecision(2) << deposit_amount << " Ԫ<Y/N>:";
    char ch;
    cin >> ch;
    if (ch != 'y' && ch != 'Y') {
        cout << "ȡ������" << endl;
        return -4;
    }
    currAccount->deposit(deposit_amount, getTimestamp(), getCurrentTime());
    cout << "�ɹ���� " << deposit_amount << " Ԫ" << endl;
    cout << "��ǰ���Ϊ " << setiosflags(ios::fixed) << setprecision(2) << currAccount->balance << " Ԫ" << endl;
    cout << "�Ƿ��ӡƾ֤<Y/N>:";
    cin >> ch;
    if (ch == 'y' || ch == 'Y')
        Record::printVoucher(currAccount->transactionHistory.back());
    Record::saveRecord(accounts);
    return 1;
}

//returnֵΪ��ͬ���͵Ĵ���Qt����
int System::withdrawal() {
    if (!currAccount) {
        cout << "Error: �˻�δ��¼" << endl;
        return -1;
    }
    string withdrawal_amount_str;
    cout << "������ȡ����:";
    cin >> withdrawal_amount_str;
    double withdrawal_amount;
    try {
        withdrawal_amount = stod(withdrawal_amount_str);
    }
    catch (...) {
        cout << "�������" << endl;
        return -1;
    }
    if (withdrawal_amount_str.length() >= 10) {
        cout << "�������" << endl;
        return -1;
    }
    if (withdrawal_amount <= 0) {
        cout << "�������" << endl;
        return -2;
    }
    if (withdrawal_amount > 10000) {
        cout << "����ȡ��ó��� 10000 Ԫ" << endl;
        return -2;
    }
    if ((int) (withdrawal_amount / 100) * 100 != withdrawal_amount) {
        cout << "ȡ�������Ϊ100��������" << endl;
        return -3;
    }
    if (withdrawal_amount > currAccount->balance) {
        cout << "����, �������Ϊ " << setiosflags(ios::fixed) << setprecision(2) << currAccount->balance
             << " Ԫ"
             << endl;
        return -4;
    }
    cout << "��ȷ�� ȡ����Ϊ " << setiosflags(ios::fixed) << setprecision(2) << withdrawal_amount << " Ԫ<Y/N>:";
    char ch;
    cin >> ch;
    if (ch != 'y' && ch != 'Y') {
        cout << "����ȡ��" << endl;
        return -5;
    }
    currAccount->withdrawal(withdrawal_amount, getTimestamp(), getCurrentTime());
    cout << "�ɹ�ȡ�� " << withdrawal_amount << " Ԫ" << endl;
    cout << "��ǰ���Ϊ " << setiosflags(ios::fixed) << setprecision(2) << currAccount->balance << " Ԫ" << endl;
    cout << "���ڳ���, ���Ժ�..." << endl;
    sleep(2);                                                           //ģ����ʵ��������ʱ��
    cout << "��ȡ�������ֽ�" << endl;
    cout << "�Ƿ��ӡƾ֤<Y/N>:";
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
    cout << "������Է�����:";
    cin >> toId;
    toId_copy = toId;
    toId = MD5(toId).toStr();
    if (!accountIndex.count(toId)) {
        cout << "���Ų�����" << endl;
        return -1;
    }
    if (toId_copy == currAccountId) {                                   //�ų�������ת�˵����
        cout << "ת�˶�����Ϊ����" << endl;
        return -3;
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
    if (transfer_amount_str.length() >= 10) {
        cout << "�������" << endl;
        return -2;
    }
    if (transfer_amount <= 0) {
        cout << "�������" << endl;
        return -3;
    }
    if (transfer_amount > 50000) {
        cout << "����ת�˽��ó��� 50000 Ԫ" << endl;
        return -3;
    }
    if (transfer_amount > currAccount->balance) {
        cout << "����, �������Ϊ " << setiosflags(ios::fixed) << setprecision(2) << currAccount->balance
             << " Ԫ"
             << endl;
        return -4;
    }
    cout << "����ת�˸� " << toId_copy << " " << setiosflags(ios::fixed) << setprecision(2) << transfer_amount
         << " Ԫ<Y/N>:";
    char ch;
    cin >> ch;
    if (ch != 'y' && ch != 'Y') {
        cout << "����ȡ��" << endl;
        return -5;
    }

    auto timestamp = getTimestamp();
    auto currTime = getCurrentTime();
    currAccount->transferOut(stod(transfer_amount_str), toId_copy, timestamp, currTime);
    accountIndex[toId]->transferIn(stod(transfer_amount_str), currAccountId, timestamp, currTime);
    cout << "�ɹ�ת�˸� " << toId_copy << " " << setiosflags(ios::fixed) << setprecision(2) << transfer_amount
         << " Ԫ"
         << endl;
    cout << "��ǰ���Ϊ " << setiosflags(ios::fixed) << setprecision(2) << currAccount->balance << " Ԫ" << endl;
    cout << "�Ƿ��ӡƾ֤<Y/N>:";
    cin >> ch;
    if (ch == 'y' || ch == 'Y')
        Record::printVoucher(currAccount->transactionHistory.back());
    Record::saveRecord(accounts);
    return 1;
}

//�������Իع�������������������
int System::PredictBalance() {
    if (currAccount->transactionHistory.size() < 20) {                          //���׼�¼С��20��ʱ�����Կ�ʼԤ��
        cout << "���׼�¼����20��,�޷�Ԥ�����" << endl;
        return 0;
    }
    float v = 0.001;
    double theta0;
    int i, j, k, m;
    int col;
    int number;
    int sum_x = 0;
    int sum_y = 0;
    int currBalance[21];
    double theta1;
    double e;
    double old_theta1;
    double old_theta0;
    Account::Transaction currtrans;
    int si = currAccount->transactionHistory.size();                           //��ȡ��ʷ���׼�¼����
    int type;
    number = 21;
    currBalance[20] = currAccount->balance;
    for (k = 19; k >= 0; k--)
    {
        currtrans = currAccount->transactionHistory[si + k - 20];
        type = currtrans.transactionType;
        if (type == 1 || type == 3) {                                          //1��3�ֱ�Ϊ����ת��
            currBalance[k] = currBalance[k + 1] - currtrans.transactionAmount;
        }
        if (type == 2 || type == 4) {                                          //2��4�ֱ�Ϊȡ���ת��
            currBalance[k] = currBalance[k + 1] + currtrans.transactionAmount;
        }
    }
    k = 0;
    m = 0;
    col = number;
    for (i = 0; i < col - 1; i++) {
        sum_x = sum_x + i;
        sum_y = sum_y + currBalance[i];
    }
    theta1 = double(sum_y / sum_x);
    theta0 = currBalance[0];
    while (true)                                                                 //��ʼ����ѭ����ֱ���ҵ����Ž��˳�ѭ��
    {
        double temp1 = 0;
        double temp0 = 0;
        for (j = 0; j < col - 1; j++) {
            temp1 = temp1 + (currBalance[j] - (theta0 + theta1 * j)) * j;
            temp0 = temp0 + (currBalance[j] - (theta0 + theta1 * j)) * 1;
        }
        old_theta1 = theta1;
        old_theta0 = theta0;
        temp1 = temp1 / col;
        temp0 = temp0 / col;
        theta1 = theta1 - v * temp1;
        theta0 = theta0 - v * temp0;
        temp0 = 0;
        temp1 = 0;
        if ((old_theta0 - theta0 < 0.05) && (old_theta1 - theta1 < 0.05)) {  //���Ԥ������Ľ��
            if (theta1 >= 0) {
                cout << "ϵͳͨ������õ������������ƣ��Ƿ�Ϊ" << theta1 << endl;
                break;
            }
            if (theta1 < 0) {
                cout << "ϵͳͨ������õ��������ʽ��ƣ�����Ϊ" << theta1 << endl;
                break;
            }
        }
    }
    return 1;
}

void System::mainMenu() {
    char choice1, choice2, choice3, choice4;
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO CursorInfo;
    GetConsoleCursorInfo(handle, &CursorInfo);//��ȡ����̨�����Ϣ
    CursorInfo.bVisible = false; //���ؿ���̨���
    SetConsoleCursorInfo(handle, &CursorInfo);//���ÿ���̨���״̬
    int index1 = 0, index2 = 0, index3 = 0, index4 = 0;
    int ch;
    do {
        cout << "+-----------------------------------------+\n";
        cout << "|           _      _____   __  __         |\n"
             << "|          / \\    |_   _| |  \\/  |        |\n"
             << "|         / _ \\     | |   | |\\/| |        |\n"
             << "|        / ___ \\    | |   | |  | |        |\n"
             << "|       /_/   \\_\\   |_|   |_|  |_|        |\n";
        cout << "|    ____            _                    |\n"
             << "|   / ___| _   _ ___| |_ ___ _ __ ___     |\n"
             << "|   \\___ \\| | | / __| __/ _ \\ '_ ` _  \\   |\n"
             << "|    ___) | |_| \\__ \\ ||  __/ | | | | |   |\n"
             << "|   |____/ \\__, |___/\\__\\___|_| |_| |_|   |\n"
             << "|          |___/                          |\n";
        cout << "|                                         |\n";
        cout << "|                                         |\n";
        cout << "|                                         |\n";
        cout << "|                                         |\n";
        cout << "|                                         |\n";
        cout << "|                                         |\n";
        cout << "|                                         |\n";
        cout << "+-----------------------------------------+\n";
        string select1[] = {"����Ա��¼", "�û���¼", "�˳����"};
        string arrow = "-> ";
        for (int i = 0; i < 3; i++) {
            goToScreen(16, 13 + 2 * i);             //���ù��λ��
            cout << select1[i];                            //���ѡ��
        }
        goToScreen(12, 13 + 2 * index1);            //���ù��λ��
        cout << arrow << "  " << select1[index1];          //�����ͷ��ѡ�����ԭ��ֻ��ѡ�

        while (true) {
            if (_kbhit()) {
                ch = _getch();                             //��ȡ���������Ҳ�����Ļ���
                if (ch == 13) {
                    break;
                } else if (ch == 72) {                      //��
                    printf("\r|%*c|\r", 41, ' ');    // /r�������ף����һ���ո񸲸ǵ�ԭ�����ݣ���/r��������
                    goToScreen(16, 13 + 2 * index1);
                    cout << select1[index1];
                    index1--;
                } else if (ch == 80) {                      //��
                    printf("\r|%*c|\r", 41, ' ');    // /r�������ף����һ���ո񸲸ǵ�ԭ�����ݣ���/r��������
                    goToScreen(16, 13 + 2 * index1);
                    cout << select1[index1];
                    index1++;
                }
                if (index1 < 0) {                           //����±�С��0����������
                    index1 = 2;
                } else if (index1 > 2) {                    //����±����2���������һλ�±꣩���������һ��
                    index1 = 0;
                }
                goToScreen(12, 13 + 2 * index1);
                cout << arrow << "  " << select1[index1];
            }
        }
        system("cls");
        switch (index1) {
            case 0:
                cout << " ----------------------------------------- \n";
                cout << "         _     ___   ____ ___ _   _ \n"
                     << "        | |   / _ \\ / ___|_ _| \\ | |\n"
                     << "        | |  | | | | |  _ | ||  \\| |\n"
                     << "        | |__| |_| | |_| || || |\\  |\n"
                     << "        |_____\\___/ \\____|___|_| \\_|\n\n";
                cout << " ----------------------------------------- \n";
                adminSignIn();
                if (!isAdmin) {
                    system("pause");
                    system("cls");
                    break;
                }
                system("cls");
                do {

                    cout << " ------------------------------------------ \n";
                    cout << "|     _          _               _         |\n"
                         << "|    / \\      __| |  _ __ ___   (_)  _ __  |\n"
                         << "|   / _ \\    / _` | | '_ ` _ \\  | | | '_ \\ |\n"
                         << "|  / ___ \\  | (_| | | | | | | | | | | | | ||\n"
                         << "| /_/   \\_\\  \\__,_| |_| |_| |_| |_| |_| |_||\n"
                         << "|                                          |\n";
                    cout << "|                                          |\n";
                    cout << "|                                          |\n";
                    cout << "|                                          |\n";
                    cout << "|                                          |\n";
                    cout << " ------------------------------------------ \n";
                    string select2[] = {"[  ]  ����", "[  ]  ����", "[  ]�޸�����", "[  ]�˳���¼"};

                    for (int i = 0; i < 4; i++) {
                        goToScreen(16, 7 + i);
                        cout << select2[i];
                    }
                    goToScreen(16, 7 + index2);
                    cout << "[��]";

                    while (true) {
                        if (_kbhit()) {
                            ch = _getch();
                            if (ch == 13) break;
                            if (ch == 72) {
                                printf("\r|%*c|\r", 42, ' ');
                                goToScreen(16, 7 + index2);
                                cout << select2[index2];
                                index2--;
                            } else if (ch == 80) {
                                printf("\r|%*c|\r", 42, ' ');
                                goToScreen(16, 7 + index2);
                                cout << select2[index2];
                                index2++;
                            }
                            if (index2 < 0) {
                                index2 = 3;
                            } else if (index2 > 3) {
                                index2 = 0;
                            }

                            goToScreen(16, 7 + index2);
                            cout << "[��]";
                        }
                    }
                    system("cls");
                    switch (index2) {
                        case 0:
                            cout << " ------------------------------------------ \n";
                            cout << "                   #����#\n";
                            cout << " ------------------------------------------ \n";
                            cout << "              #¼���˻���Ϣ#\n";
                            signUp();
                            system("pause");
                            system("cls");
                            break;
                        case 1:
                            cout << " ------------------------------------------ \n";
                            cout << "                  #����#\n\n";
                            deleteAccount();
                            system("pause");
                            system("cls");
                            break;
                        case 2:
                            cout << " ------------------------------------------ \n";
                            cout << "                 #�޸�����#\n\n";
                            changePassword();
                            system("pause");
                            system("cls");
                            break;
                        case 3:
                            signOut();
                            //system ("pause");
                            system("cls");
                            break;
                        default:
                            cout << "�����������������!\n";
                            system("pause");
                            system("cls");
                    }
                } while (index2 != 3);
                break;
            case 1:
                cout << " ----------------------------------------- \n";
                cout << "         _     ___   ____ ___ _   _ \n"
                     << "        | |   / _ \\ / ___|_ _| \\ | |\n"
                     << "        | |  | | | | |  _ | ||  \\| |\n"
                     << "        | |__| |_| | |_| || || |\\  |\n"
                     << "        |_____\\___/ \\____|___|_| \\_|\n\n";
                cout << " ----------------------------------------- \n";
                signIn();
                if (!currAccount) {
                    system("pause");
                    system("cls");
                    continue;
                }
                system("cls");
                do {
                    cout << "+--------------------------------------------+\n";
                    cout << "| __        __   _                           |\n"
                         << "| \\ \\      / /__| | ___ ___  _ __ ___   ___  |\n"
                         << "|  \\ \\ /\\ / / _ \\ |/ __/ _ \\| '_ ` _ \\ / _ \\ |\n"
                         << "|   \\ V  V /  __/ | (_| (_) | | | | | |  __/ |\n"
                         << "|    \\_/\\_/ \\___|_|\\___\\___/|_| |_| |_|\\___| |\n"
                         << "|                                            |\n";
                    cout << "|                                            |\n";
                    cout << "|                                            |\n";
                    cout << "|                                            |\n";
                    cout << "|                                            |\n";
                    cout << "|                                            |\n";
                    cout << "|                                            |\n";
                    cout << "|                                            |\n";
                    cout << "|                                            |\n";
                    cout << "|                                            |\n";
                    cout << "|                                            |\n";
                    cout << "+--------------------------------------------+\n";
                    string select3[] = {"[  ]  ���", "[  ]  ȡ��", "[  ]  ת��", "[  ]��ѯ���", "[  ]���׼�¼",
                                        "[  ]���Ԥ��", "[  ]�޸�����", "[  ]  ����", "[  ]  �˳�"};
                    goToScreen(15, 7);
                    cout << "�û�: " << currAccount->name;
                    for (int i = 0; i < 8; i++) {
                        goToScreen(15, 9 + i);
                        cout << select3[i];
                    }
                    goToScreen(15, 9 + index3);
                    cout << "[��]";

                    while (true) {
                        if (_kbhit()) {
                            ch = _getch();//not print at screen
                            if (ch == 13) {
                                break;
                            } else if (ch == 72) {//up
                                printf("\r|%*c|\r", 44, ' ');
                                goToScreen(15, 9 + index3);
                                cout << select3[index3];
                                index3--;
                            } else if (ch == 80) {//down
                                printf("\r|%*c|\r", 44, ' ');
                                goToScreen(15, 9 + index3);
                                cout << select3[index3];
                                index3++;
                            }
                            if (index3 < 0) {
                                index3 = 7;
                            } else if (index3 > 7) {
                                index3 = 0;
                            }

                            goToScreen(15, 9 + index3);
                            cout << "[��]";
                        }
                    }
                    system("cls");
                    switch (index3) {
                        case 0:
                            cout << " ------------------------------------------ \n";
                            cout << "                   #���#\n\n";
                            deposit();
                            system("pause");
                            system("cls");
                            break;
                        case 1:
                            cout << " ------------------------------------------ \n";
                            cout << "                 #ȡ��#\n\n";
                            withdrawal();
                            system("pause");
                            system("cls");
                            break;
                        case 2:
                            cout << " ------------------------------------------ \n";
                            cout << "                 #ת��#\n\n";
                            transfer();
                            system("pause");
                            system("cls");
                            break;
                        case 3:
                            cout << " ------------------------------------------ \n";
                            cout << "                 #���#\n\n";
                            showBalance();
                            system("pause");
                            system("cls");
                            break;
                        case 4:
                            cout << " ------------------------------------------ \n";
                            cout << "                 #���׼�¼#\n\n";
                            Record::exportTransactionHistory(currAccount->transactionHistory);
                            system("pause");
                            system("cls");
                            break;
                        case 5:
                            cout << " ------------------------------------------ \n";
                            cout << "                 #���Ԥ��#\n\n";
                            PredictBalance();
                            system("pause");
                            system("cls");
                            break;
                        case 6:
                            cout << " ------------------------------------------ \n";
                            cout << "                #�޸�����#\n\n";
                            changePassword();
                            system("pause");
                            system("cls");
                            break;
                        case 7:
                            cout << " ------------------------------------------ \n";
                            cout << "                  #����#\n\n";
                            deleteAccount();
                            system("pause");
                            system("cls");
                            choice3 = '0';
                            break;
                        case 8:
                            signOut();
                            system("cls");
                            break;
                    }
                } while (index3 != 7);
                break;
            case 2:
                cout << "   ____                 _ ____            /\\/|\n"
                     << "  / ___| ___   ___   __| | __ ) _   _  __|/\\/ \n"
                     << " | |  _ / _ \\ / _ \\ / _` |  _ \\| | | |/ _ \\   \n"
                     << " | |_| | (_) | (_) | (_| | |_) | |_| |  __/   \n"
                     << "  \\____|\\___/ \\___/ \\__,_|____/ \\__, |\\___|   \n"
                     << "                                |___/         \n";
                sleep(1);
                exit(0);
                break;
            default:
                printf("�����������������!\n");
                system("pause");
                system("cls");
        }
    } while (true);
}

string System::getTimestamp() {
    timeb timestamp{};
    ftime(&timestamp);

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
