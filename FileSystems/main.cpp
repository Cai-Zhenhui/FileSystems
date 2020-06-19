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
            printf("文件号:%d\n", fOpen(&par[0], FM_Write));
        }
        else if (cmd.compare("seek") == 0) {
            DWORD offset;
            DWORD fileId;
            cin >> fileId >> offset;
            cout << fSeek(fileId, offset) << endl;
        }
        else if (cmd.compare("read") == 0) {
            //cin >> par;
            DWORD fileId;
            int size;
            cin >> fileId >> size;
            char* buffer = new char[size+1];
            memset(buffer, 0, size + 1);

            cout<<"实际读取字节数:"<<fRead(buffer, size, 1, fileId)<<endl;

            printf("%s\n", buffer);
            delete[] buffer;
        }
        else if (cmd.compare("write") == 0) {
            DWORD fileId;
            cin >> fileId >> par;

            cout << "实际写入字节数:" << fWrite(&par[0], par.length() + 1, 1, fileId);

        }
        else if (cmd.compare("close") == 0) {
            DWORD fileId;
            cin >> fileId;
            fClose(fileId);
        }
        else if (cmd.compare("mkdir") == 0) {
            cin >> par;
            dCreate(&par[0]);
        }
        else if (cmd.compare("rm") == 0) {
            cin >> par;
            bool isDeleteDir=0;
            bool isRecursive=1;
            cin >> isDeleteDir >> isRecursive;
            dDelete(&par[0], isDeleteDir, isRecursive);
        }
        else if (cmd.compare("dir") == 0) {
            showDir();
        }
        else if (cmd.compare("tree") == 0) {
            showDirTree();
        }
        else if (cmd.compare("format") == 0) {
            DWORD diskId, partitionId;
            cin >> diskId >> partitionId;
            cout << diskManagement.format(diskId - 1, partitionId - 1) << endl;
        }
        
        cout << endl;
        showCurrentDirectory();
    }

    users.logout(user);
	return 0;
}