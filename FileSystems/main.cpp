//cmd命令行接口
#include "stdafx.h"
#include <iostream>
#include "Interface.h"
#include <conio.h>
using namespace std;

void intputPassword(char* password) {
    unsigned char c;
    int i = 0;

    while ((c = getch()) != '\r')
    {
        if (i < MAXPasswordLength && isprint(c))
        {
            password[i++] = c;
            putchar('*');
        }
        else if (i > 0 && c == '\b')
        {
            --i;
            putchar('\b');
            putchar(' ');
            putchar('\b');
        }
    }
    putchar('\n');
    password[i] = '\0';
    fflush(stdin);
}

int main() {

    User user = { 0 };
    {
        int times = 0;
        do
        {
            cout << "请输入用户名:";
            cin >> user.username;
            cout << "请输入密码:";
            intputPassword(user.password);
            ++times;
            if (!users.login(user)) {
                cout << "用户名或密码错误！" << endl;
                Sleep(3000);
                system("cls");
            }
            else {
                system("cls");
                break;
            }
        } while (times < 3);
    }

    if (!users.isLogin) {
        cout << "错误超过3次系统退出!" << endl;
        system("pause");
        return 0;
    }
    if (users.currentUser.id != 0) {
        dIntoSub(user.username);//进入普通用户目录
    }
    showCurrentDirectory();

    std::string cmd;
    std::string par;
    while (cin>>cmd)
    {
        if (cmd.compare("cd") == 0) {
            cin >> par;
            dIntoSub(&par[0]);
        }
        else if(cmd.compare("create") == 0){
            cin >> par;
            fCreate(&par[0]);
        }
        else if (cmd.compare("del") == 0) {
            cin >> par;
            fDelete(&par[0]);
        }
        else if (cmd.compare("open") == 0) {
            cin >> par;
            //printf("文件号:%d\n", fopen(&par[0]));
        }
        else if (cmd.compare("read") == 0) {

        }
        else if (cmd.compare("write") == 0) {

        }
        else if (cmd.compare("close") == 0) {

        }
        else if (cmd.compare("mkdir") == 0) {

        }
        else if (cmd.compare("rm") == 0) {

        }
        else if (cmd.compare("dir") == 0) {

        }
        else if (cmd.compare("tree") == 0) {
            showDirTree();
        }
        
        cout << endl;
        showCurrentDirectory();
    }

    users.logout(user);
	return 0;
}