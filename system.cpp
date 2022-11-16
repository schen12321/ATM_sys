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
    passwd = MD5(passwd).toStr();                               //密码以md5形式存储
    if (accountIndex[id]->passwd != passwd) {
        cout << "密码错误";
        accountIndex[id]->wrongPasswdLeft -= 1;
        Record::saveRecord(accounts);
        switch (accountIndex[id]->wrongPasswdLeft) {                    //输错3次判断冻结账户
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
    currAccount->wrongPasswdLeft = 3;                                   //重置剩余错误次数
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
    string id, name, passwd;
    cout << "请输入卡号:";                               //输入卡号姓名密码
    cin >> id;
    if(id.length() != 6){                              //限制位数和形式
        cout << "卡号必须为6位数字" << endl;
        return -3;
    }
    for(int i =0 ; i < id.length() ; i++){
        if(id[i] < 48 || id[i] > 57){
            cout << "卡号必须为6位数字" << endl;
            return -4;
        }
    }
    id = MD5(id).toStr();
    if (accountIndex.count(id)) {
        cout << "此卡号已存在" << endl;
        return 0;
    }
    cout << "请输入开户人姓名:";
    cin >> name;
    cout << "请输入密码(6位数字):";
    cin >> passwd;
    if (passwd.length() != 6) {                         //限制位数和形式
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
    Account account(id, name, passwd);        //增加用户成功
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
        if (newPasswd.length() != 6) {                          //限制位数和形式
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
        if (newPasswd == accountIndex[id]->passwd) {            //判断新旧密码是否相同
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
    //非管理员操作部分
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
    Record::saveRecord(accounts);                               //保存记录
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
        if (ch != 'y' && ch != 'Y') {                               //确认操作
            cout << "操作取消" << endl;
            return -2;
        }
        swap(*accountIndex[id], *(accounts.end() - 1));
        accounts.pop_back();
        accountIndex.erase(id);
        for (auto &acc: accounts) {
            accountIndex[acc.id] = &(acc);
        }
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
             << " 元, 请确认是否注销<Y/N>:";                            //显示余额以确认是否注销
        cin >> ch;
        if (ch != 'y' && ch != 'Y') {
            cout << "操作取消";
            return -2;
        }
        accountIndex.erase(currAccount->id);                      //注销成功
        swap(*currAccount, *(accounts.end() - 1));
        accounts.pop_back();
        currAccount = nullptr;                                       //当前用户退出p
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

//return值为不同类型的错误，Qt备用
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
    catch (...) {                                                       //输入数字以外字符导致的错误
        cout << "输入错误" << endl;
        return -1;
    }
    if (deposit_amount_str.length() >= 10) {                            //防止金额过大导致的溢出
        cout << "输入错误" << endl;
        return -2;
    }
    if (deposit_amount <= 0) {                                          //不允许为负数
        cout << "输入错误" << endl;
        return -2;
    }
    if (deposit_amount > 10000) {                                       //单笔限额
        cout << "单笔存款不得超过 10000 元" << endl;
        return -2;
    }
    if ((int) (deposit_amount / 100) * 100 != deposit_amount) {         //限制为100的整数倍
        cout << "存款金额必须为100的整数倍" << endl;
        return -3;
    }
    cout << "请确认 存款金额为 " << setiosflags(ios::fixed) << setprecision(2) << deposit_amount << " 元<Y/N>:";
    char ch;
    cin >> ch;
    if (ch != 'y' && ch != 'Y') {
        cout << "取消操作" << endl;
        return -4;
    }
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

//return值为不同类型的错误，Qt备用
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
        return -1;
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
    sleep(2);                                                           //模拟真实场景出钞时间
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
    if (toId_copy == currAccountId) {                                   //排除给自身转账的情况
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
        return -2;
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

//利用线性回归计算算出余额走向的趋势
int System::PredictBalance() {
    if (currAccount->transactionHistory.size() < 20) {                          //交易记录小于20条时不可以开始预测
        cout << "交易记录少于20条,无法预测余额" << endl;
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
    int si = currAccount->transactionHistory.size();                           //获取历史交易记录条数
    int type;
    number = 21;
    currBalance[20] = currAccount->balance;
    for (k = 19; k >= 0; k--)
    {
        currtrans = currAccount->transactionHistory[si + k - 20];
        type = currtrans.transactionType;
        if (type == 1 || type == 3) {                                          //1和3分别为存款和转入
            currBalance[k] = currBalance[k + 1] - currtrans.transactionAmount;
        }
        if (type == 2 || type == 4) {                                          //2和4分别为取款和转出
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
    while (true)                                                                 //开始迭代循环，直到找到最优解退出循环
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
        if ((old_theta0 - theta0 < 0.05) && (old_theta1 - theta1 < 0.05)) {  //输出预测分析的结果
            if (theta1 >= 0) {
                cout << "系统通过运算得到您的余额呈涨势，涨幅为" << theta1 << endl;
                break;
            }
            if (theta1 < 0) {
                cout << "系统通过运算得到您的余额呈降势，降幅为" << theta1 << endl;
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
    GetConsoleCursorInfo(handle, &CursorInfo);//获取控制台光标信息
    CursorInfo.bVisible = false; //隐藏控制台光标
    SetConsoleCursorInfo(handle, &CursorInfo);//设置控制台光标状态
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
        string select1[] = {"管理员登录", "用户登录", "退出软件"};
        string arrow = "-> ";
        for (int i = 0; i < 3; i++) {
            goToScreen(16, 13 + 2 * i);             //设置光标位置
            cout << select1[i];                            //输出选项
        }
        goToScreen(12, 13 + 2 * index1);            //设置光标位置
        cout << arrow << "  " << select1[index1];          //输出箭头和选项（覆盖原先只有选项）

        while (true) {
            if (_kbhit()) {
                ch = _getch();                             //读取键盘输入且不在屏幕输出
                if (ch == 13) {
                    break;
                } else if (ch == 72) {                      //上
                    printf("\r|%*c|\r", 41, ' ');    // /r返回行首，输出一串空格覆盖掉原先内容，再/r返回行首
                    goToScreen(16, 13 + 2 * index1);
                    cout << select1[index1];
                    index1--;
                } else if (ch == 80) {                      //下
                    printf("\r|%*c|\r", 41, ' ');    // /r返回行首，输出一串空格覆盖掉原先内容，再/r返回行首
                    goToScreen(16, 13 + 2 * index1);
                    cout << select1[index1];
                    index1++;
                }
                if (index1 < 0) {                           //如果下标小于0，则调至最后
                    index1 = 2;
                } else if (index1 > 2) {                    //如果下标大于2（数组最后一位下标），则调到第一个
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
                    string select2[] = {"[  ]  开户", "[  ]  销户", "[  ]修改密码", "[  ]退出登录"};

                    for (int i = 0; i < 4; i++) {
                        goToScreen(16, 7 + i);
                        cout << select2[i];
                    }
                    goToScreen(16, 7 + index2);
                    cout << "[■]";

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
                            cout << "[■]";
                        }
                    }
                    system("cls");
                    switch (index2) {
                        case 0:
                            cout << " ------------------------------------------ \n";
                            cout << "                   #开户#\n";
                            cout << " ------------------------------------------ \n";
                            cout << "              #录入账户信息#\n";
                            signUp();
                            system("pause");
                            system("cls");
                            break;
                        case 1:
                            cout << " ------------------------------------------ \n";
                            cout << "                  #销户#\n\n";
                            deleteAccount();
                            system("pause");
                            system("cls");
                            break;
                        case 2:
                            cout << " ------------------------------------------ \n";
                            cout << "                 #修改密码#\n\n";
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
                            cout << "输入错误，请重新输入!\n";
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
                    string select3[] = {"[  ]  存款", "[  ]  取款", "[  ]  转账", "[  ]查询余额", "[  ]交易记录",
                                        "[  ]余额预测", "[  ]修改密码", "[  ]  销户", "[  ]  退出"};
                    goToScreen(15, 7);
                    cout << "用户: " << currAccount->name;
                    for (int i = 0; i < 8; i++) {
                        goToScreen(15, 9 + i);
                        cout << select3[i];
                    }
                    goToScreen(15, 9 + index3);
                    cout << "[■]";

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
                            cout << "[■]";
                        }
                    }
                    system("cls");
                    switch (index3) {
                        case 0:
                            cout << " ------------------------------------------ \n";
                            cout << "                   #存款#\n\n";
                            deposit();
                            system("pause");
                            system("cls");
                            break;
                        case 1:
                            cout << " ------------------------------------------ \n";
                            cout << "                 #取款#\n\n";
                            withdrawal();
                            system("pause");
                            system("cls");
                            break;
                        case 2:
                            cout << " ------------------------------------------ \n";
                            cout << "                 #转账#\n\n";
                            transfer();
                            system("pause");
                            system("cls");
                            break;
                        case 3:
                            cout << " ------------------------------------------ \n";
                            cout << "                 #余额#\n\n";
                            showBalance();
                            system("pause");
                            system("cls");
                            break;
                        case 4:
                            cout << " ------------------------------------------ \n";
                            cout << "                 #交易记录#\n\n";
                            Record::exportTransactionHistory(currAccount->transactionHistory);
                            system("pause");
                            system("cls");
                            break;
                        case 5:
                            cout << " ------------------------------------------ \n";
                            cout << "                 #余额预测#\n\n";
                            PredictBalance();
                            system("pause");
                            system("cls");
                            break;
                        case 6:
                            cout << " ------------------------------------------ \n";
                            cout << "                #修改密码#\n\n";
                            changePassword();
                            system("pause");
                            system("cls");
                            break;
                        case 7:
                            cout << " ------------------------------------------ \n";
                            cout << "                  #销户#\n\n";
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
                printf("输入错误，请重新输入!\n");
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
