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
    cout << "请输入您的卡号:";
    cin >> id;
    id_bak = id;
    id = MD5(id).toStr();
    if (!accountIndex.count(id)) {
        cout << "卡号不存在" << endl;
        return -1;
    }
    if (!accountIndex[id]->wrongPasswdLeft) {
        cout << "您的账户已被冻结" << endl;
        return -2;
    }
    cout << "请输入您的密码:";
    cin >> passwd;
    passwd = MD5(passwd).toStr();
    if (accountIndex[id]->passwd != passwd) {
        cout << "密码错误";
        accountIndex[id]->wrongPasswdLeft -= 1;
        Record::saveRecord(accounts);
        switch (accountIndex[id]->wrongPasswdLeft) {
            case 2:
                cout << ", 还有2次机会" << endl;
                return -3;
            case 1:
                cout << ", 还有1次机会" << endl;
                return -4;
            case 0:
                cout << ", 您的账户已被冻结, 请联系管理员解冻" << endl;
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
        cout << "您的账户已退出" << endl;
        return 1;
    } else if (isAdmin) {
        isAdmin = false;
        cout << "管理员账户已退出" << endl;
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
        cout << "此卡号已存在" << endl;
        return 0;
    }
    cout << "请输入开户人姓名:";
    cin >> name;
    cout << "请输入密码(6位数字):";
    cin >> passwd;
    if (passwd.length() != 6) {
        cout << "密码必须为6位数字" << endl;
        return -1;
    }
    for (int i = 0; i < 6; i++) {
        if (passwd[i] < 48 || passwd[i] > 57) {
            cout << "密码必须为6位数字" << endl;
            return -2;
        }
    }
    passwd = MD5(passwd).toStr();

    //增加用户成功
    Account account(id, name, passwd);
    accounts.push_back(account);
    for (auto &acc: accounts) {
        accountIndex[acc.id] = &(acc);
    }
    Record::saveRecord(accounts);
    cout << "开户成功" << endl;
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
            cout << "卡号不存在" << endl;
            return 0;
        }
        cout << "请输入新密码(6位数字):";
        cin >> newPasswd;
        if (newPasswd.length() != 6) {
            cout << "密码必须为6位数字" << endl;
            return -1;
        }
        for (int i = 0; i < 6; i++) {
            if (newPasswd[i] < 48 || newPasswd[i] > 57) {
                cout << "密码必须为6位数字" << endl;
                return -2;
            }
        }
        newPasswd = MD5(newPasswd).toStr();
        if (newPasswd == accountIndex[id]->passwd) {
            cout << "新旧密码相同" << endl;
            return 0;
        }
        cout << "请确认是否修改<Y/N>:";
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
        cout << "请输入新密码(6位数字):";
        cin >> newPasswd;
        if (newPasswd.length() != 6) {
            cout << "密码必须为6位数字" << endl;
            return -1;
        }
        for (int i = 0; i < 6; i++) {
            if (newPasswd[i] < 48 || newPasswd[i] > 57) {
                cout << "密码必须为6位数字" << endl;
                return -2;
            }
        }
        cout << "请再次确认新密码:";
        cin >> passwdConfirm;
        if (passwdConfirm != newPasswd) {
            cout << "两次输入的密码不相同" << endl;
            return 0;
        }
        newPasswd = MD5(newPasswd).toStr();
        if (newPasswd == currAccount->passwd) {
            cout << "新旧密码相同" << endl;
            return 0;
        }
        cout << "请确认是否修改<Y/N>:";
        cin >> ch;
        if (ch != 'y' && ch != 'Y') {
            cout << "操作取消" << endl;
            return 0;
        }
        currAccount->passwd = newPasswd;
    }
    Record::saveRecord(accounts);//保存
    cout << "密码修改成功" << endl;
    return 1;
}

//账户注销
int System::deleteAccount() {
    if (isAdmin) {
        string id;
        char ch;
        cout << "请输入要注销的卡号:";
        cin >> id;
        id = MD5(id).toStr();
        if (!accountIndex.count(id)) {
            cout << "卡号不存在" << endl;
            return -1;
        }
        cout << "请确认是否注销<Y/N>:";
        cin >> ch;
        if (ch != 'y' && ch != 'Y') {
            cout << "操作取消" << endl;
            return -2;
        }
        swap(*accountIndex[id], *(accounts.end() - 1));
        accounts.pop_back();
        accountIndex.erase(id);
        Record::saveRecord(accounts);
        cout << "账户注销成功" << endl;
        return 1;
    } else {
        string passwd;
        char ch;
        cout << "请输入密码:";
        cin >> passwd;
        passwd = MD5(passwd).toStr();
        if (passwd != currAccount->passwd) {
            cout << "密码错误";
            return -1;
        }
        cout << "您的余额为 " << setiosflags(ios::fixed) << setprecision(2) << currAccount->balance
             << " 元, 请确认是否注销<Y/N>:";
        cin >> ch;
        if (ch != 'y' && ch != 'Y') {
            cout << "操作取消";
            return -2;
        }
        //成功注销
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
    cout << "您当前的余额为 " << setiosflags(ios::fixed) << setprecision(2) << currAccount->balance << " 元" << endl;
    return 1;
}

    int System::deposit() {
        if (!currAccount) {
            cout << "Error: 账户未登录" << endl;
            return -1;
        }
        string deposit_amount_str;
        cout << "请输入存款金额:";
        cin >> deposit_amount_str;
        double deposit_amount;
        try {
            deposit_amount = stod(deposit_amount_str);
        }
        catch (...) {
            cout << "输入错误" << endl;
            return -1;
        }
        if (deposit_amount_str.length() >= 10) {
            cout << "输入错误" << endl;
        }
        if (deposit_amount <= 0) {
            cout << "输入错误" << endl;
            return -2;
        }
        if (deposit_amount > 10000) {
            cout << "单笔存款不得超过 10000 元" << endl;
            return -2;
        }
        if ((int) (deposit_amount / 100) * 100 != deposit_amount) {
            cout << "存款金额必须为100的整数倍" << endl;
            return -3;
        }
        cout << "请确认 存款金额为 " << setiosflags(ios::fixed) << setprecision(2) << deposit_amount << " 元<Y/N>:";
        char ch;
        cin >> ch;
        if (ch != 'y' && ch != 'Y')
            cout << "取消操作" << endl;
        currAccount->deposit(deposit_amount, getTimestamp(), getCurrentTime());
        cout << "成功存款 " << deposit_amount << " 元" << endl;
        cout << "当前余额为 " << setiosflags(ios::fixed) << setprecision(2) << currAccount->balance << " 元" << endl;
        cout << "是否打印凭证<Y/N>:";
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
        cout << "请输入取款金额:";
        cin >> withdrawal_amount_str;
        double withdrawal_amount;
        try {
            withdrawal_amount = stod(withdrawal_amount_str);
        }
        catch (...) {
            cout << "输入错误" << endl;
            return -1;
        }
        if (withdrawal_amount_str.length() >= 10) {
            cout << "输入错误" << endl;
        }
        if (withdrawal_amount <= 0) {
            cout << "输入错误" << endl;
            return -2;
        }
        if (withdrawal_amount > 10000) {
            cout << "单笔取款不得超过 10000 元" << endl;
            return -2;
        }
        if ((int) (withdrawal_amount / 100) * 100 != withdrawal_amount) {
            cout << "取款金额必须为100的整数倍" << endl;
            return -3;
        }
        if (withdrawal_amount > currAccount->balance) {
            cout << "余额不足, 您的余额为 " << setiosflags(ios::fixed) << setprecision(2) << currAccount->balance
                 << " 元"
                 << endl;
            return -4;
        }
        cout << "请确认 取款金额为 " << setiosflags(ios::fixed) << setprecision(2) << withdrawal_amount << " 元<Y/N>:";
        char ch;
        cin >> ch;
        if (ch != 'y' && ch != 'Y') {
            cout << "操作取消" << endl;
            return -5;
        }
        currAccount->withdrawal(withdrawal_amount, getTimestamp(), getCurrentTime());
        cout << "成功取款 " << withdrawal_amount << " 元" << endl;
        cout << "当前余额为 " << setiosflags(ios::fixed) << setprecision(2) << currAccount->balance << " 元" << endl;
        cout << "正在出钞, 请稍后..." << endl;
        sleep(2);
        cout << "请取回您的现金" << endl;
        cout << "是否打印凭证<Y/N>:";
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
        cout << "请输入对方卡号:";
        cin >> toId;
        toId_copy = toId;
        toId = MD5(toId).toStr();
        if (!accountIndex.count(toId)) {
            cout << "卡号不存在" << endl;
            return -1;
        }
        if (toId_copy == currAccountId) {
            cout << "转账对象不能为自身" << endl;
            return -3;
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
        if (transfer_amount_str.length() >= 10) {
            cout << "输入错误" << endl;
        }
        if (transfer_amount <= 0) {
            cout << "输入错误" << endl;
            return -3;
        }
        if (transfer_amount > 50000) {
            cout << "单笔转账金额不得超过 50000 元" << endl;
            return -3;
        }
        if (transfer_amount > currAccount->balance) {
            cout << "余额不足, 您的余额为 " << setiosflags(ios::fixed) << setprecision(2) << currAccount->balance
                 << " 元"
                 << endl;
            return -4;
        }
        cout << "您将转账给 " << toId_copy << " " << setiosflags(ios::fixed) << setprecision(2) << transfer_amount
             << " 元<Y/N>:";
        char ch;
        cin >> ch;
        if (ch != 'y' && ch != 'Y') {
            cout << "操作取消" << endl;
            return -5;
        }

        auto timestamp = getTimestamp();
        auto currTime = getCurrentTime();
        currAccount->transferOut(stod(transfer_amount_str), toId_copy, timestamp, currTime);
        accountIndex[toId]->transferIn(stod(transfer_amount_str), currAccountId, timestamp, currTime);
        cout << "成功转账给 " << toId_copy << " " << setiosflags(ios::fixed) << setprecision(2) << transfer_amount
             << " 元"
             << endl;
        cout << "当前余额为 " << setiosflags(ios::fixed) << setprecision(2) << currAccount->balance << " 元" << endl;
        cout << "是否打印凭证<Y/N>:";
        cin >> ch;
        if (ch == 'y' || ch == 'Y')
            Record::printVoucher(currAccount->transactionHistory.back());
        Record::saveRecord(accounts);
        return 1;
    }

    void System::mainMenu() {
        char choice1, choice2, choice3, choice4;
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
            cout << "|             【1】 管理员登录\t          |\n";
            cout << "|                                         |\n";
            cout << "|             【2】 用户登录\t          |\n";
            cout << "|                                         |\n";
            cout << "|             【0】 退出软件\t          |\n";
            cout << "|                                         |\n";
            cout << "+-----------------------------------------+\n";
            cin >> choice1;
            cin.sync(); //清空输入缓冲区
            system ("cls");
            switch (choice1) {
                case '3':
                    system("mode con cols=120 lines=42");
                    try{
                        snakeGame game;
                    }
                    catch(runtime_error &gameEnd) { //结束
                        system("cls");
                    }
                    break;
                case '1':
                    cout << " ----------------------------------------- \n";
                    cout << "         _     ___   ____ ___ _   _ \n"
                         << "        | |   / _ \\ / ___|_ _| \\ | |\n"
                         << "        | |  | | | | |  _ | ||  \\| |\n"
                         << "        | |__| |_| | |_| || || |\\  |\n"
                         << "        |_____\\___/ \\____|___|_| \\_|\n\n";
                    cout << " ----------------------------------------- \n";
                    adminSignIn();
                    if (!isAdmin) {
                        system ("pause");
                        system ("cls");
                        break;
                    }
                    system ("cls");
                    do {
                        cout << " ----------------------------------------- \n";
                        cout << "     _          _               _         \n"
                             << "    / \\      __| |  _ __ ___   (_)  _ __  \n"
                             << "   / _ \\    / _` | | '_ ` _ \\  | | | '_ \\ \n"
                             << "  / ___ \\  | (_| | | | | | | | | | | | | |\n"
                             << " /_/   \\_\\  \\__,_| |_| |_| |_| |_| |_| |_|\n"
                             << "                                          \n";
                        cout << "              【1】 开户\n";
                        cout << "              【2】 销户\n";
                        cout << "              【3】 修改密码\n";
                        cout << "              【0】 退出登录\n\n";
                        cout << " ----------------------------------------- \n";
                        cin >> choice2;
                        cin.sync(); //清空输入缓冲区
                        system ("cls");
                        switch (choice2) {
                            case '1':
                                cout << " ------------------------------------------ \n";
                                cout << "                   #开户#\n";
                                cout << " ------------------------------------------ \n";
                                cout << "              #录入账户信息#\n";
                                signUp();
                                system ("pause");
                                system ("cls");
                                break;
                            case '2':
                                cout << " ------------------------------------------ \n";
                                cout << "                  #销户#\n\n";
                                deleteAccount();
                                system ("pause");
                                system ("cls");
                                break;
                            case '3':
                                cout << " ------------------------------------------ \n";
                                cout << "                 #修改密码#\n\n";
                                changePassword();
                                system ("pause");
                                system ("cls");
                                break;
                            case '0':
                                signOut();
                                //system ("pause");
                                system ("cls");
                                break;
                            default:
                                cout << "输入错误，请重新输入!\n";
                                system ("pause");
                                system ("cls");
                        }
                    } while (choice2 != '0');
                    break;
                case '2':
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
                        cout << "|               用户：" << currAccount->name << "\t                     |" << endl;
                        cout << "|              【1】 存款\t             |\n";
                        cout << "|              【2】 取款\t             |\n";
                        cout << "|              【3】 转账\t             |\n";
                        cout << "|              【4】 账户查询\t             |\n";
                        cout << "|              【5】 修改密码\t             |\n";
                        cout << "|              【6】 销户\t             |\n";
                        cout << "|              【0】 退出\t             |\n";
                        cout << "+--------------------------------------------+\n";
                        cin >> choice3;
                        cin.sync(); //清空输入缓冲区
                        system ("cls");
                        switch (choice3) {
                            case '1':
                                cout << " ------------------------------------------ \n";
                                cout << "                   #存款#\n\n";
                                deposit();
                                system ("pause");
                                system ("cls");
                                break;
                            case '2':
                                cout << " ------------------------------------------ \n";
                                cout << "                 #取款#\n\n";
                                withdrawal();
                                system ("pause");
                                system ("cls");
                                break;
                            case '3':
                                cout << " ------------------------------------------ \n";
                                cout << "                 #转账#\n\n";
                                transfer();
                                system ("pause");
                                system ("cls");
                                break;
                            case '4':
                                do {
                                    cout << " ------------------------------------------ \n\n";
                                    cout << "                   #查询#\n\n";
                                    cout << "                【1】 余额\n";
                                    cout << "                【2】 交易记录\n";
                                    cout << "                【0】 返回\n\n";
                                    cout << " ------------------------------------------ \n\n";
                                    cin >> choice4;
                                    cin.sync(); //清空输入缓冲区
                                    system ("cls");
                                    switch (choice4) {
                                        case '1':
                                            cout << " ------------------------------------------ \n";
                                            cout << "                 #余额#\n\n";
                                            showBalance();
                                            system ("pause");
                                            system ("cls");
                                            break;
                                        case '2':
                                            cout << " ------------------------------------------ \n";
                                            cout << "                 #交易记录#\n\n";
                                            Record::exportTransactionHistory(currAccount->transactionHistory);
                                            system ("pause");
                                            system ("cls");
                                            break;
                                        case '0':
                                            system ("cls");
                                            break;
                                        default:
                                            cout << "输入错误，请重新输入!\n";
                                            system ("pause");
                                            system ("cls");
                                    }
                                } while (choice4 != '0');
                                break;
                            case '5':
                                cout << " ------------------------------------------ \n";
                                cout << "                #修改密码#\n\n";
                                changePassword();
                                system ("pause");
                                system ("cls");
                                break;
                            case '6':
                                cout << " ------------------------------------------ \n";
                                cout << "                  #销户#\n\n";
                                deleteAccount();
                                system ("pause");
                                system ("cls");
                                choice3 = '0';
                                break;
                            case '0':
                                signOut();
                                system ("cls");
                                break;
                            default:
                                cout << "输入错误，请重新输入!\n";
                                system ("pause");
                                system ("cls");
                                break;
                        }
                    } while (choice3 != '0');
                    break;
                case '0':
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
                    printf("输入错误，请重新输入!\n");
                    system ("pause");
                    system ("cls");
            }
        } while (choice1 != '0');
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
