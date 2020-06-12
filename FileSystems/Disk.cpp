#include "stdafx.h"
#include <ctime>
#include "Disk.h"
std::vector<DWORD> keyList;
void readList() {
    WIN32_FIND_DATA wfd;
    std::tstring path(DiskPath);
    path += TEXT("*");
    path += ExpandName;

    HANDLE handle = FindFirstFile(path.c_str(), &wfd);
    if (handle == INVALID_HANDLE_VALUE) {
        Println("Error. GetLastError:%d", GetLastError());
    }
    else {
        do
        {
            Println("%s", wfd.cFileName);
            std::tstring file(DiskPath);
            file += wfd.cFileName;
            FILE* pFile = tfopen(file.c_str(), TEXT("rb"));
            if (pFile == NULL) {
                Println("���ļ�ʧ�ܣ�GetLastError:%d", GetLastError());
            }
            DiskHeader dh;
            fread(&dh, sizeof(dh), 1, pFile);
            fclose(pFile);
            if (!dh.isValid()) {
                continue;
            }
            keyList.push_back(dh.dwKey);

        } while (FindNextFile(handle, &wfd));
    }
    FindClose(handle);

}

inline DWORD generateKey() {
    DWORD tmpKey = (DWORD)time(NULL);
    std::vector<DWORD>::iterator it = keyList.begin();
    bool isExist = false;
    for (; it != keyList.end(); ++it) {
        if (tmpKey == (*it)) {
            isExist = true;
            break;
        }
    }
    if (isExist) {
        tmpKey += 1;
    }
    return tmpKey;
}


Disk::Disk()
{
}

Disk::~Disk()
{
}

bool Disk::format(DWORD partitions)
{
    if (pFile == NULL) {
        return false;
    }
    //д�������

    return true;
}

bool Disk::create(__int64 size)
{
    if (pFile != NULL) {
        save();//�ȱ���ԭ�д���
    }
    readList();
    if (size < BlockSize * 10) {
        return false;
    }
    blocks = size / BlockSize;//������

    dh.dwKey = generateKey();
    dh.dwSizeHigh = (DWORD)(size >> 32);
    dh.dwSizeLow = (DWORD)size;

    std::tstring fileName(DiskPath);
    TCHAR tmpbuffer[260];
    tsprintf(tmpbuffer, TEXT("%d"), dh.dwKey);
    fileName += tmpbuffer;
    fileName += ExpandName;

    BYTE block[BlockSize] = { 0 };

    pFile = tfopen(fileName.c_str(), TEXT("wb"));

    //д������ļ�ͷ
    fwrite(&dh, sizeof(dh), 1, pFile);

    //д����̷����� Ĭ��1������

    size /= BlockSize;
    for (__int64 i = 0; i < size; ++i) {
        fwrite(block, sizeof(block), 1, pFile);
    }
    fclose(pFile);
    Sleep(1000);
    return 0;
}


