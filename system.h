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
    vector<Account> accounts;                                        //记录所有用户数据
    unordered_map<string, Account *> accountIndex;                   //以卡号为键创建用户索引
    Account *currAccount{nullptr};                                   //当前登录用户，为空则表示未登录
    string currAccountId;                                            //当前用户卡号，为空则表示未登录
    bool isAdmin{false};                                             //是否为管理员账号
    const string adminId{"1"};                              //保存管理员账户ID
    const string adminPassword{"1"};                            //保存管理员账户密码

public:
    //类的构造函数，从文件中加载账户信息
    System();

    //启动ATM模拟器，之后无循环，直到点击退出按钮使用exit(0)结束程序
    void start();

private:
    //负责实现账户登录功能
    int signIn();

    //负责实现管理登录功能
    int adminSignIn();

    //负责实现账户退出功能
    int signOut();

    //负责实现管理员增加账户功能
    int signUp();

    //负责实现修改密码功能
    int changePassword();

    //负责实现账户注销功能（选）
    int deleteAccount();

    //负责实现存款功能
    int deposit();

    //负责实现取款功能
    int withdrawal();

    //负责实现转账功能
    int transfer();

    //负责实现余额查询功能
    int showBalance();

    //负责实现菜单的显示
    void mainMenu();

    //获取时间戳
    static string getTimestamp();

    //获取格式化的当前时间（如："2021-09-09 22:02:35"）
    static string getCurrentTime();
};
