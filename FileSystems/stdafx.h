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

//常量

constexpr auto BlockSize = (1 * 1024);
#define MAXFileNameLength (((sizeof(SubFileControlBlock)+16)/16+1)*16-sizeof(SubFileControlBlock))
//#define SIZE_FCB (MAXFileNameLength + sizeof(SubFileControlBlock))
#define SIZE_FCB sizeof(FileControlBlock)
#define MAXItem (BlockSize / SIZE_FCB)
#define MAXUsernameLength 20
#define MAXPasswordLength 20
#define DELETE_FLAG 0xE5

#pragma pack(1)
//文件类型
enum FILETYPE
{
	FT_FILE,//文件
	FT_DIRECTORY//目录
};
struct User
{
	BYTE id;
	char username[MAXUsernameLength];
	char password[MAXPasswordLength];
};
typedef struct User User;
struct PhysicalAddress
{
	BYTE bIdDisk;//磁盘特征码
	BYTE bIdPartition;//分区id
	DWORD dwBlockStart;//文件起始块号
	DWORD dwBlockEnd;//文件终止块号
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
	//利用率40/56
	struct Date() {
		//Println("调用Date构造");
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

	//一共19个字符
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
//	文件控制块在内存中的子结构2 去除文件名的剩余部分
struct SubFileControlBlock
{
	BYTE bOwner;//该文件所属
	FILETYPE fileType;//文件类型
	//存取权限
	//PhysicalAddress physicalAddress;//文件物理地址
	DWORD dwIndexFAT;//在FAT表中起始项
	DWORD dwFileLength;//文件长度单位字节
	WORD wLinkTimes;//连接计数
	Date timeCreate;//创建时间
	Date timeModify;//修改时间
	Date timeAccess;//访问时间

	SubFileControlBlock();
	
	//用struct FileControlBlock来初始化
	SubFileControlBlock(struct FileControlBlock& obj2);
	SubFileControlBlock& operator=(struct FileControlBlock& obj2);
};
typedef struct SubFileControlBlock SubFileControlBlock;

//Description:
//	文件控制块在内存中的子结构1 文件名部分
struct IndexItemMem
{
	char fileName[MAXFileNameLength];

	DWORD fileId;//文件号 //文件打开之后的添加的新内容
	BYTE bFileMode;
	PhysicalAddress physicalAddress;//文件物理地址

	//函数
	IndexItemMem();

	//用struct FileControlBlock来初始化
	IndexItemMem(struct FileControlBlock& obj2);
	IndexItemMem& operator=(struct FileControlBlock& obj2);
	IndexItemMem& operator=(char * szfileName2);
	bool operator==(char* fileName2);
};
typedef struct IndexItemMem IndexItemMem;

//Description:
//	完整的文件控制块 也是在磁盘中存储的结构
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
		if (size >= BlockSize || size <= 0) {
			return 0;
		}
		if (nPos + size >= BlockSize) {
			size = (BlockSize - 1) - nPos;
		}
		memcpy(Des, buffer + nPos, size);
		nPos += size;
		return size;
	}
	int write(void* Src, int size) {
		if (size >= BlockSize || size <= 0) {
			return 0;
		}
		if (nPos + size >= BlockSize) {
			size = (BlockSize - 1) - nPos;
		}
		memcpy(buffer + nPos, Src, size);
		nPos += size;
		return size;
	}

	//更改块的游标位置
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
			//越界
			return -1;
		}
		return nPos = newPos;
	}

	//Description:
	//	将该块拷贝到pBlock,pBlock游标将指向初始位置
	//Paramter: 
	//	_In_ struct Block* pBlock Des
	//Return Value:
	//	true 执行成功
	//	false 执行失败
	void copy(_In_ struct Block* pBlock) {
		memcpy(pBlock->buffer, buffer, BlockSize);
		pBlock->nPos = 0;
	}
};
#pragma pack()

#include "DiskManagement.h"
//磁盘管理
extern DiskManagement diskManagement;