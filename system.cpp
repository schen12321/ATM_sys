#include "system.h"


System::System() {
    Record::loadRecord(accounts);
    for (auto &account: accounts)
        accountIndex[account.id] = &account;
}

//启动ATM，之后无限循环，直到点击退出按钮使用exit(0)结束程序
void System::start() {
        mainMenu();
}

int System::signIn() {
    string id, id_bak, passwd;
    cout << "请输入卡号:";
    cin >> id;
    id_bak = id;
    id = MD5(id).toStr();
    if (!accountIndex.count(id)) {
        cout << "账户不存在" << endl;
        return -1;
    }
    if (!accountIndex[id]->wrongPasswdLeft) {
        cout << "账户已冻结" << endl;
        return -2;
    }
    cout << "请输入密码:";
    cin >> passwd;
    passwd = MD5(passwd).toStr();
    cout << accountIndex[id]->wrongPasswdLeft;
    if (accountIndex[id]->passwd != passwd) {
        cout << "密码错误";
        accountIndex[id]->wrongPasswdLeft -= 1;
        Record::saveRecord(accounts);
        switch (accountIndex[id]->wrongPasswdLeft) {
            case 2:
                cout << " , 还有2次机会" << endl;
                return -3;
            case 1:
                cout << ", 还有1次机会" << endl;
                return -4;
            case 0:
                cout << ", 账户已冻结, 请联系管理员解冻" << endl;
                return -5;
        }
    }
    cout << "登陆成功" << endl;
    currAccount = accountIndex[id];
    currAccountId = id_bak;
    currAccount->wrongPasswdLeft = 3;        //重置剩余错误次数
    return 1;
}

int System::adminSignIn() {
    string id, id_bak, passwd;
    cout << "请输入管理员账户ID:";
    cin >> id;
    id_bak = id;
    if (id == adminId) {
        cout << "请输入管理员账户密码:";
        cin >> passwd;
        if (passwd != adminPassword) {
            cout << "密码错误" << endl;
            return -1;
        }
        cout << "管理员登陆成功" << endl;
        isAdmin = true;
        return 1;
    }
    cout << "非管理员ID" << endl;
    return -2;
}

int System::signOut() {
    if (currAccount) {
        currAccount = nullptr;
        currAccountId.clear();
        cout << "账户退出成功" << endl;
        return 1;
    } else if (isAdmin) {
        isAdmin = false;
        cout << "管理员账户退出成功" << endl;
        return 2;
    }
    cout << "Error: 账户未登录" << endl;
    return -1;
}

int System::signUp() {
    //输入卡号姓名密码
    string id, name, passwd;
    cout << "请输入卡号:";
    cin >> id;
    id = MD5(id).toStr();
    if (accountIndex.count(id)) {
        cout << "此卡号已存在！" << endl;
        return 0;
    }
    cout << "请输入你的姓名:";
    cin >> name;
    cout << "请输入密码:";
    cin >> passwd;
    passwd = MD5(passwd).toStr();

    //增加用户成功
    Account account(id, name, passwd);
    accounts.push_back(account);
    for (auto &acc: accounts) {
        accountIndex[acc.id] = &(account);
    }
    Record::saveRecord(accounts);
    cout << "用户增加成功!" << endl;
    return 1;
}

//修改密码
int System::changePassword() {
    //管理员操作部分
    if (isAdmin) {
        string id, newPasswd;
        char ch;
        cout << "请输入要修改的卡号:";
        cin >> id;
        id = MD5(id).toStr();
        if (!accountIndex.count(id)) {
            cout << "卡号不存在！" << endl;
            return 0;
        }
        cout << "请输入新密码:";
        cin >> newPasswd;
        newPasswd = MD5(newPasswd).toStr();
        if (newPasswd == accountIndex[id]->passwd) {
            cout << "新旧密码相同!" <<endl;
            return 0;
        }
        cout << "请确认是否修改<Y/N>: ";
        cin >> ch;
        if (ch != 'y' && ch != 'Y') {
            cout << "操作取消" << endl;
            return 0;
        }
        accountIndex[id]->wrongPasswdLeft = 3;
        accountIndex[id]->passwd = newPasswd;
    }
        //非管理员
    else {
        string newPasswd, passwdConfirm;
        char ch;
        cout << "请输入新密码: ";
        cin >> newPasswd;
        cout << "请再次确认新密码: ";
        cin >> passwdConfirm;
        if (passwdConfirm != newPasswd) {
            cout << "两次输入的密码不相同!" << endl;
            return 0;
        }
        newPasswd = MD5(newPasswd).toStr();
        if (newPasswd == currAccount->passwd) {
            cout << "新密码与旧密码相同!" << endl;
            return 0;
        }
        cout << "请确认是否修改<Y/N>: ";
        cin >> ch;
        if (ch != 'y' && ch != 'Y') {
            cout << "操作取消" << endl;
            return 0;
        }
        currAccount->passwd = newPasswd;
    }
    Record::saveRecord(accounts);//保存
    cout << "密码修改成功!" << endl;
    return 1;
}

//账户注销
int System::deleteAccount() {
    if (isAdmin) {
        string id;
        char ch;
        cout << "请输入要注销的账户id: ";
        cin >> id;
        id = MD5(id).toStr();
        if (!accountIndex.count(id)) {
            cout << "账户不存在" << endl;
            return -1;
        }
        cout << "请确认是否要注销该账户<Y/N>: ";
        cin >> ch;
        if (ch != 'y' && ch != 'Y') {
            cout << "操作取消" << endl;
            return -2;
        }

        /*for (auto &account: accounts) {
            accountIndex[account.id] = &(account);
        }*/
        swap(*accountIndex[id], *(accounts.end() - 1));
        accounts.pop_back();
        accountIndex.erase(id);
        Record::saveRecord(accounts);
        cout << "账户注销成功" << endl;
        return 1;
    }
    else {
        string passwd;
        char ch;
        cout << "请输入密码: ";
        cin >> passwd;
        passwd = MD5(passwd).toStr();
        if (passwd != currAccount->passwd){
            cout << "密码错误";
            return -1;
        }
        cout << "您的余额为 " <<setiosflags(ios::fixed) << setprecision(2)<< currAccount->balance << " 元,是否确认注销<Y/N>";
        cin >> ch;
        if (ch != 'y' && ch != 'Y'){
            cout << "操作取消";
            return -2;
        }
        //成功注销
        /*for (auto &account: accounts) {
            accountIndex[account.id] = &(account);
        }*/
        accountIndex.erase(currAccount->id);
        swap(*currAccount, *(accounts.end() - 1));
        accounts.pop_back();
        currAccount = nullptr;
        currAccountId.clear();
        Record::saveRecord(accounts);
        cout << "账户注销成功" << endl;
        return 1;
    }
}

int System::showBalance() {
    if (isAdmin) {
        string id;
        cout << "请输入需要查看的卡号: ";
        cin >> id;
        id = MD5(id).toStr();
        if (!accountIndex.count(id)) {
            cout << "卡号不存在" << endl;
            return 0;
        }
        cout << "该账号当前的余额为 " << setiosflags(ios::fixed) << setprecision(2) << accountIndex[id]->balance
             << " 元";
    }
    else {
        cout << "您当前的余额为 " << setiosflags(ios::fixed) << setprecision(2) << currAccount->balance << " 元";
    }
    return 1;
}

int System::deposit() {
    if (!currAccount) {
        cout << "Error: 账户未登录" << endl;
        return -1;
    }
    string deposit_amount_str;
    cout << "请输入存款金额: " << endl;
    cin >> deposit_amount_str;
    double deposit_amount;
    try {
        deposit_amount = stod(deposit_amount_str);
    }
    catch (...) {
        cout << "输入错误" << endl;
        return -1;
    }
    if (deposit_amount <= 0) {
        cout << "存款金额不能为0" << endl;
        return -2;
    }
    if ((int) (deposit_amount / 100) * 100 != deposit_amount) {
        cout << "存款金额必须为100的整数倍" << endl;
        return -3;
    }
    cout << "请确认 存款金额为 " << setiosflags(ios::fixed) << setprecision(2) << deposit_amount << " 元<Y>" << endl;
    char ch;
    cin >> ch;
    if (ch != 'y' && ch != 'Y')
        cout << "取消操作" << endl;
    currAccount->deposit(deposit_amount, getTimestamp(), getCurrentTime());
    cout << "成功存款 " << deposit_amount << " 元" << endl;
    cout << "您的余额为 " << setiosflags(ios::fixed) << setprecision(2) << currAccount->balance << " 元" << endl;
    cout << "是否打印凭证<Y/N>" << endl;
    cin >> ch;
    if (ch == 'y' || ch == 'Y')
        Record::printVoucher(currAccount->transactionHistory.back());
    Record::saveRecord(accounts);
    return 1;
}

int System::withdrawal() {
    if (!currAccount) {
        cout << "Error: 账户未登录" << endl;
        return -1;
    }
    string withdrawal_amount_str;
    cout << "请输入取款金额: " << endl;
    cin >> withdrawal_amount_str;
    double withdrawal_amount;
    try {
        withdrawal_amount = stod(withdrawal_amount_str);
    }
    catch (...) {
        cout << "输入错误" << endl;
        return -1;
    }
    if (withdrawal_amount <= 0) {
        cout << "取款金额不能为0" << endl;
        return -2;
    }
    if ((int) (withdrawal_amount / 100) * 100 != withdrawal_amount) {
        cout << "取款金额必须为100的整数倍" << endl;
        return -3;
    }
    if (withdrawal_amount > currAccount->balance) {
        cout << "余额不足，您的余额为 " << setiosflags(ios::fixed) << setprecision(2) << currAccount->balance << " 元"
             << endl;
        return -4;
    }
    cout << "请确认 取款金额为 " << setiosflags(ios::fixed) << setprecision(2) << withdrawal_amount << " 元<Y>" << endl;
    char ch;
    cin >> ch;
    if (ch != 'y' && ch != 'Y') {
        cout << "取消操作" << endl;
        return -5;
    }
    currAccount->withdrawal(withdrawal_amount, getTimestamp(), getCurrentTime());
    cout << "成功取款 " << withdrawal_amount << " 元" << endl;
    cout << "您的余额为 " << setiosflags(ios::fixed) << setprecision(2) << currAccount->balance << " 元" << endl;
    cout << "请收好您的取款" << endl;
    cout << "是否打印凭证<Y/N>" << endl;
    cin >> ch;
    if (ch == 'y' || ch == 'Y')
        Record::printVoucher(currAccount->transactionHistory.back());
    Record::saveRecord(accounts);
    return 1;
}

int System::transfer() {
    if (!currAccount) {
        cout << "Error: 账户未登录" << endl;
        return -1;
    }
    string toId, toId_copy;
    cout << "请输入对方账户:";
    cin >> toId;
    toId_copy = toId;
    toId = MD5(toId).toStr();
    if (!accountIndex.count(toId)) {
        cout << "对方账户不存在" << endl;
        return -1;
    }
    if (toId_copy == currAccountId) {
        cout << "转账对象不能为自身" << endl;
    }
    string transfer_amount_str;
    double transfer_amount;
    cout << "请输入转账金额:";
    cin >> transfer_amount_str;
    try {
        transfer_amount = stod(transfer_amount_str);
    }
    catch (...) {
        cout << "输入错误" << endl;
        return -2;
    }
    if (transfer_amount <= 0) {
        cout << "转账金额不能为0" << endl;
        return -3;
    }
    if (transfer_amount > currAccount->balance) {
        cout << "余额不足，您的余额为 " << setiosflags(ios::fixed) << setprecision(2) << currAccount->balance << " 元"
             << endl;
        return -4;
    }
    cout << "将转账给 " << toId_copy << " " << setiosflags(ios::fixed) << setprecision(2) << transfer_amount << " 元<Y>"
         << endl;
    char ch;
    cin >> ch;
    if (ch != 'y' && ch != 'Y') {
        cout << "取消操作" << endl;
        return -5;
    }

    auto timestamp = getTimestamp();
    auto currTime = getCurrentTime();
    currAccount->transferOut(stod(transfer_amount_str), toId_copy, timestamp, currTime);
    accountIndex[toId]->transferIn(stod(transfer_amount_str), currAccountId, timestamp, currTime);
    cout << "成功转账给 " << toId_copy << " " << setiosflags(ios::fixed) << setprecision(2) << transfer_amount << " 元"
         << endl;
    cout << "您的余额为 " << setiosflags(ios::fixed) << setprecision(2) << currAccount->balance << " 元" << endl;
    cout << "是否打印凭证<Y/N>" << endl;
    cin >> ch;
    if (ch == 'y' || ch == 'Y')
        Record::printVoucher(currAccount->transactionHistory.back());
    Record::saveRecord(accounts);
    return 1;
}

void System::mainMenu() {
    int choice1, choice2, choice3, choice4;
    do {
        cout << "        ┌------------------------┐\n";
        cout << "            #欢迎使用ATM仿真软件#\n\n";
        cout << "              【1】 管理员登录     \n";
        cout << "              【2】 用户登录     \n";
        cout << "              【0】 退出软件     \n\n";
        cout << "        └------------------------┘\n";
        cin >> choice1;
        switch (choice1) {
            case 1:
                adminSignIn();
                if (!isAdmin) {
                    continue;
                }
                do {
                    cout << "        ┌------------------------┐\n";
                    cout << "                 #账户管理#\n\n";
                    cout << "              【1】 开户\n";
                    cout << "              【2】 销户\n";
                    cout << "              【3】 修改密码\n";
                    cout << "              【0】 退出登录\n\n";
                    cout << "        └------------------------┘\n";
                    cin >> choice2;
                    switch (choice2) {
                        case 1:
                            cout << "        ┌------------------------┐\n";
                            cout << "                   #开户#\n";
                            cout << "        └------------------------┘\n";
                            cout << "              #录入账户信息#\n";
                            signUp();
                            break;
                        case 2:
                            cout << "        ┌------------------------┐\n";
                            cout << "                  #销户#\n\n";
                            deleteAccount();
                            break;
                        case 3:
                            cout << "        ┌------------------------┐\n";
                            cout << "                 #修改密码#\n\n";
                            changePassword();
                            break;
                        case 0:
                            signOut();
                            break;
                        default:
                            cout << "输入错误，请重新输入!\n";
                    }
                } while (choice2 != 0);
                break;
            case 2:
                signIn();
                if (!currAccount) {
                    continue;
                }
                do {
                    cout << "        ┌------------------------┐\n";
                    cout << "              #欢迎" << currAccount->name << "#\n\n";
                    cout << "              【1】 存款\n";
                    cout << "              【2】 取款\n";
                    cout << "              【3】 账户查询\n";
                    cout << "              【4】 转账\n";
                    cout << "              【5】 修改密码\n";
                    cout << "              【6】 销户\n";
                    cout << "              【0】 退出\n\n";
                    cout << "        └------------------------┘\n";
                    cin >> choice3;
                    switch (choice3) {
                        case 1:
                            cout << "        ┌------------------------┐\n";
                            cout << "                  #存款#\n\n";
                            deposit();
                            break;
                        case 2:
                            cout << "        ┌------------------------┐\n";
                            cout << "                 #取款#\n\n";
                            withdrawal();
                            break;
                        case 3:
                            do {
                                cout << "        ┌------------------------┐\n";
                                cout << "                  #查询#\n\n";
                                cout << "               【1】 余额\n";
                                cout << "               【2】 交易记录\n";
                                cout << "               【0】 返回\n\n";
                                cout << "        └------------------------┘\n";
                                cin >> choice4;
                                switch (choice4) {
                                    case 1:
                                        cout << "        ┌------------------------┐\n";
                                        cout << "                 #余额#\n\n";
                                        showBalance();
                                        break;
                                    case 2:
                                        cout << "        ┌------------------------┐\n";
                                        cout << "                 #交易记录#\n\n";
                                        Record::exportTransactionHistory(currAccount->transactionHistory);
                                        break;
                                    case 0:
                                        break;
                                    default:
                                        cout << "输入错误，请重新输入!\n";
                                }
                            } while (choice4 != 0);
                            break;
                        case 4:
                            cout << "        ┌------------------------┐\n";
                            cout << "                 #转账#\n\n";
                            transfer();
                            break;
                        case 5:
                            cout << "        ┌------------------------┐\n";
                            cout << "                #修改密码#\n\n";
                            changePassword();
                            break;
                        case 6:
                            cout << "        ┌------------------------┐\n";
                            cout << "                  #销户#\n\n";
                            deleteAccount();
                            break;
                        case 0:
                            signOut();
                            break;
                        default:
                            cout << "输入错误，请重新输入!\n";
                    }
                } while (choice3 != 0);
                break;
            case 0:
                printf("感谢您的使用!\n");
                break;
            default:
                printf("输入错误，请重新输入!\n");
        }
    } while (choice1 != 0);
    exit(0);
}

string System::getTimestamp() {
    timeb timestamp{};
    ftime(&timestamp);

    //毫秒数可能不足三位，要补上0
    stringstream ss;
    ss << setw(3) << setfill('0') << timestamp.millitm;

    return to_string(timestamp.time) + ss.str();
}

//获取格式化的当前时间（如："2021-09-09 22:02:35"）
string System::getCurrentTime() {
    //这两个变量用于获取当前时间哟
    tm currTime{};
    time_t timestamp = time(nullptr);

    localtime_s(&currTime, &timestamp);

    //用于储存最终格式化的结果
    string ans;
    //用于格式化当前时间
    stringstream ss;

    //年
    ans += to_string(1900 + currTime.tm_year) + "-";

    //月
    ss << setw(2) << setfill('0') << 1 + currTime.tm_mon;
    ans += ss.str() + "-";

    //日
    ss.str("");
    ss << setw(2) << setfill('0') << currTime.tm_mday;
    ans += ss.str() + " ";

    //时
    ss.str("");
    ss << setw(2) << setfill('0') << currTime.tm_hour;
    ans += ss.str() + ":";

    //分
    ss.str("");
    ss << setw(2) << setfill('0') << currTime.tm_min;
    ans += ss.str() + ":";

    //秒
    ss.str("");
    ss << setw(2) << setfill('0') << currTime.tm_sec;
    ans += ss.str();
    return ans;
}