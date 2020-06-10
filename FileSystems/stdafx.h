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

//常量

constexpr auto BlockSize = 1*1024;

//文件类型
enum FILETYPE
{
	FT_FILE,//文件
	FT_DIRECTORY//目录
};
struct User
{
	BYTE id;
	std::tstring username;
	std::tstring password;
};
typedef struct User User;
struct PhysicalAddress
{
	DWORD dwKeyDisk;//磁盘特征码
	BYTE bIdPartition;//分区id
	DWORD dwBlockStart;//起始块号
	DWORD dwBlockEnd;//终止块号
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
};
typedef struct Date Date;

struct IndexItemDisk
{
	BYTE bOwner;//该文件所属
	FILETYPE fileType;//文件类型
	//存取权限
	PhysicalAddress physicalAddress;//文件物理地址
	DWORD dwFileLength;//文件长度
	WORD wLinkTimes;//连接计数
	Date timeCreate;//创建时间
	Date timeModify;//修改时间
	Date timeAccess;//访问时间
};
typedef struct IndexItemDisk IndexItemDisk;

struct IndexItemMem
{
	IndexItemDisk iid;//内存索引项
	DWORD dwIndexItemMemId;//索引节点编号
	//状态
	//访问计数
	//文件所属文件系统的逻辑设备号 
	//链接指针：指向空闲链表和散列队列
};
typedef struct IndexItemMem IndexItemMem;

struct Directory
{
	std::tstring fileName;
	IndexItemMem* pIndexItemMem;
};
typedef struct Directory Directory;

struct Block
{
	BYTE buffer[BlockSize];
	bool isLoad;
	struct Block* nextBlock;
};