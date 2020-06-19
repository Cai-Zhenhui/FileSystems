#pragma once
#include <string>
#include <Windows.h>
#include <vector>
#include <ctime>
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

constexpr auto BlockSize = (1 * 1024);
#define MAXFileNameLength (((sizeof(SubFileControlBlock)+16)/16+1)*16-sizeof(SubFileControlBlock))
//#define SIZE_FCB (MAXFileNameLength + sizeof(SubFileControlBlock))
#define SIZE_FCB sizeof(FileControlBlock)
#define MAXItem (BlockSize / SIZE_FCB)
#define MAXUsernameLength 20
#define MAXPasswordLength 20
#define DELETE_FLAG 0xE5

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
	char username[MAXUsernameLength];
	char password[MAXPasswordLength];
	bool operator==(User& obj2);
	bool operator==(char* username);
	bool operator==(std::string strUserName);
};
typedef struct User User;
struct PhysicalAddress
{
	BYTE bIdDisk;//����id
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
	struct Date() {
		//Println("����Date����");
		tm* ptm;
		time_t t = time(NULL);
		ptm = localtime(&t);
		wYear = ptm->tm_year + 1900;
		bMonth = ptm->tm_mon + 1;
		bDay = ptm->tm_mday;
		bHour = ptm->tm_hour;
		bMinute = ptm->tm_min;
		bSecond = ptm->tm_sec;
	}

	//һ��19���ַ�
	//mm/dd/yyyy hh:mm:ss
	void sprint(char* buffer) {
		if (buffer == NULL) {
			return;
		}
		sprintf(buffer, "%02d/%02d/%04d %02d:%02d:%02d", this->bMonth, this->bDay, this->wYear,
			this->bHour, this->bMinute, this->bSecond);
	}
};
typedef struct Date Date;


//Description:
//	�ļ����ƿ����ڴ��е��ӽṹ2 ȥ���ļ�����ʣ�ಿ��
struct SubFileControlBlock
{
	BYTE bOwner;//���ļ�����
	FILETYPE fileType;//�ļ�����
	//��ȡȨ��
	//PhysicalAddress physicalAddress;//�ļ������ַ
	DWORD dwIndexFAT;//��FAT������ʼ��
	DWORD dwFileLength;//�ļ����ȵ�λ�ֽ�
	WORD wLinkTimes;//���Ӽ���
	Date timeCreate;//����ʱ��
	Date timeModify;//�޸�ʱ��
	Date timeAccess;//����ʱ��

	SubFileControlBlock();
	
	//��struct FileControlBlock����ʼ��
	SubFileControlBlock(struct FileControlBlock& obj2);
	SubFileControlBlock& operator=(struct FileControlBlock& obj2);
};
typedef struct SubFileControlBlock SubFileControlBlock;

//Description:
//	�ļ����ƿ����ڴ��е��ӽṹ1 �ļ�������
struct IndexItemMem
{
	char fileName[MAXFileNameLength];

	DWORD fileId;//�ļ��� //�ļ���֮�����ӵ�������
	BYTE bFileMode;
	DWORD nPos;
	PhysicalAddress physicalAddress;//�ļ������ַ

	//����
	IndexItemMem();

	//��struct FileControlBlock����ʼ��
	IndexItemMem(struct FileControlBlock& obj2);
	IndexItemMem& operator=(struct FileControlBlock& obj2);
	IndexItemMem& operator=(char * szfileName2);
	bool operator==(char* fileName2);
	bool operator==(DWORD fileId2);
};
typedef struct IndexItemMem IndexItemMem;

//Description:
//	�������ļ����ƿ� Ҳ���ڴ����д洢�Ľṹ
struct FileControlBlock
{
	char fileName[MAXFileNameLength];
	SubFileControlBlock sFCB;

	FileControlBlock();
	FileControlBlock(char* fileName2);
	FileControlBlock(const char* fileName2);
	FileControlBlock(struct IndexItemMem& iimobj2, struct SubFileControlBlock& sfcbobj2);

	bool isValid();
	bool operator==(struct FileControlBlock& obj2);
	bool operator==(char* fileName2);
	struct FileControlBlock& operator=(struct IndexItemMem& obj2);
	struct FileControlBlock& operator=(struct SubFileControlBlock& obj2);
};
typedef struct FileControlBlock FileControlBlock;


struct Block
{
	BYTE buffer[BlockSize];
	//bool isLoad;
	//struct Block* nextBlock;
	int nPos;
	struct Block() {
		memset(this, 0, sizeof(*this));
	}
	int read(void* Des, int size) {
		if (size > BlockSize || size <= 0) {
			return 0;
		}
		if (nPos + size > BlockSize) {
			size = BlockSize - nPos;
		}
		memcpy(Des, buffer + nPos, size);
		nPos += size;
		return size;
	}
	int write(void* Src, int size) {
		if (size > BlockSize || size <= 0) {
			return 0;
		}
		if (nPos + size > BlockSize) {
			size = BlockSize - nPos;
		}
		memcpy(buffer + nPos, Src, size);
		nPos += size;
		return size;
	}

	//���Ŀ���α�λ��
	int setPos(_In_ long _Offset, _In_ int _Origin = SEEK_SET) {
		int newPos = 0;
		switch (_Origin)
		{
		case SEEK_SET: {
			newPos = _Offset;
			break;
		}
		case SEEK_CUR: {
			newPos = nPos + _Offset;
			break;
		}
		case SEEK_END: {
			newPos = (BlockSize - 1) + _Offset;
			break;
		}
		}

		if (newPos / (BlockSize - 1) != 0) {
			//Խ��
			return -1;
		}
		return nPos = newPos;
	}
	void clear() {
		memset(this, 0, sizeof(*this));
	}

	//Description:
	//	���ÿ鿽����pBlock,pBlock�α꽫ָ���ʼλ��
	//Paramter: 
	//	_In_ struct Block* pBlock Des
	//Return Value:
	//	true ִ�гɹ�
	//	false ִ��ʧ��
	void copy(_In_ struct Block* pBlock) {
		memcpy(pBlock->buffer, buffer, BlockSize);
		pBlock->nPos = 0;
	}
};
#pragma pack()

#include "DiskManagement.h"
//���̹���
extern DiskManagement diskManagement;

#include "User.h"
extern Users users;