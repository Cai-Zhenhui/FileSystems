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
    std::string par2;
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
        else if (cmd.compare("search") == 0) {
            cin >> par;
            fSearch(&par[0]);
        }
        else if (cmd.compare("open") == 0) {
            cin >> par;
            printf("�ļ���:%d\n", fOpen(&par[0], FM_Write));
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

            cout<<"ʵ�ʶ�ȡ�ֽ���:"<<fRead(buffer, size, 1, fileId)<<endl;

            printf("%s\n", buffer);
            delete[] buffer;
        }
        else if (cmd.compare("write") == 0) {
            DWORD fileId;
            cin >> fileId >> par;

            cout << "ʵ��д���ֽ���:" << fWrite(&par[0], par.length() + 1, 1, fileId);

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
        else if (cmd == "createDisk") {
            DWORD blocks;
            cin >> blocks;
            cout << diskManagement.generateDisk(BlockSize * blocks) << endl;
        }
        else if (cmd == "rename") {
            cin >> par >> par2;
            cout << fRename(&par[0],&par2[0]) << endl;
        }
        
        cout << endl;
        showCurrentDirectory();
    }

    users.logout(user);
	return 0;
}