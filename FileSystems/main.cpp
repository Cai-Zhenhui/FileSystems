//cmd�����нӿ�
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
            cout << "�������û���:";
            cin >> user.username;
            cout << "����������:";
            intputPassword(user.password);
            ++times;
            if (!users.login(user)) {
                cout << "�û������������" << endl;
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
        cout << "���󳬹�3��ϵͳ�˳�!" << endl;
        system("pause");
        return 0;
    }
    if (users.currentUser.id != 0) {
        dIntoSub(user.username);//������ͨ�û�Ŀ¼
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
            //printf("�ļ���:%d\n", fopen(&par[0]));
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