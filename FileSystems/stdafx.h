#pragma once
#include <string>
#include <Windows.h>
#include <vector>
#include "Debug.h"

#ifdef UNICODE
#define tstring wstring
#define tsprintf wsprintf
#define tvsprintf wvsprintf
#define __TFILE__ __FILEW__
#define __TFUNCTION__ __FUNCTIONW__
#else
#define tstring string
#define tsprintf sprintf
#define tvsprintf vsprintf
#define __TFILE__ __FILE__
#define __TFUNCTION__ __FUNCTION__
#endif // !UNICODE

//����

constexpr auto BlockSize = 1*1024;
#define MAXFileNameLength 32
#define MAXUsernameLength 20
#define MAXPasswordLength 20

#pragma pack(1)
//�ļ�����
enum FILETYPE
{
	FT_FILE,//�ļ�
	FT_DIRECTORY//Ŀ¼
};
struct User
{
	BYTE id;
	TCHAR username[MAXUsernameLength];
	TCHAR password[MAXPasswordLength];
};
typedef struct User User;
struct PhysicalAddress
{
	DWORD dwKeyDisk;//����������
	BYTE bIdPartition;//����id
	DWORD dwBlockStart;//�ļ���ʼ���
	DWORD dwBlockEnd;//�ļ���ֹ���
};
typedef struct PhysicalAddress PhysicalAddress;

struct Date
{
	WORD wYear;//year 0-9999 14bit
	BYTE bMonth;////month 1-12 4bit
	BYTE bDay;//day 1-31 5bit

	BYTE bHour;//hour 0-23 5bit
	BYTE bMinute;//min 0-59 6bit
	BYTE bSecond;//sec 0-59 6bit
	//������40/56
};
typedef struct Date Date;

struct IndexItemDisk
{
	BYTE bOwner;//���ļ�����
	FILETYPE fileType;//�ļ�����
	//��ȡȨ��
	PhysicalAddress physicalAddress;//�ļ������ַ
	DWORD dwFileLength;//�ļ�����
	WORD wLinkTimes;//���Ӽ���
	Date timeCreate;//����ʱ��
	Date timeModify;//�޸�ʱ��
	Date timeAccess;//����ʱ��
};
typedef struct IndexItemDisk IndexItemDisk;

struct IndexItemMem
{
	IndexItemDisk iid;//�ڴ�������
	DWORD dwIndexItemMemId;//�����ڵ���
	//״̬
	//���ʼ���
	//�ļ������ļ�ϵͳ���߼��豸�� 
	//����ָ�룺ָ����������ɢ�ж���
};
typedef struct IndexItemMem IndexItemMem;

struct FileControlBlock
{
	TCHAR fileName[MAXFileNameLength];
	IndexItemMem* pIndexItemMem;
};
typedef struct FileControlBlock FileControlBlock;

struct Block
{
	BYTE buffer[BlockSize];
	bool isLoad;
	struct Block* nextBlock;
};
#pragma pack()