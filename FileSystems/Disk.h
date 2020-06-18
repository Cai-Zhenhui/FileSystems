#pragma once
#include "Partition.h"
#pragma pack(1)
struct DiskHeader
{
	WORD wFlag;//DF
	DWORD dwKey;//key唯一标识磁盘特征码
	DWORD dwSizeLow;//磁盘大小低32
	DWORD dwSizeHigh;//磁盘大小高32
	DWORD dwPartitions;//分区数量
	struct DiskHeader() {
		memset(this, 0, sizeof(*this));
		wFlag = 'FD';
	}
	bool isValid() {
		return wFlag == 'FD';
	}
};
typedef struct DiskHeader DiskHeader;
#pragma pack()

#define DiskPath TEXT(".\\Disk\\")
#define ExpandName TEXT(".disk")

//	磁盘类将一个磁盘文件加载到内存中，维护这段内存来模拟磁盘
class Disk
{
public:
	DiskHeader dh;
	BYTE bIdDisk;
	DWORD blocks;
	FILE* pFile;
	Partition* currentPartition;
	Disk();
	~Disk();

	//Description:
	//	从指定物理地址读取一个块
	//Paramter: 
	//	_In_ PhysicalAddress pa 物理地址
	//	_In_ DWORD blockId 块号
	//	_Out_ Block* block 指向块的地址
	//Return Value:
	//	true 执行成功
	//	false 执行失败
	bool readBlock(_In_ PhysicalAddress pa, _In_ DWORD blockId, _Out_ Block* block);

	//Description:
	//	从指将一个块写入指定物理地址
	//Paramter: 
	//	_In_ PhysicalAddress pa 物理地址
	//	_In_ DWORD blockId 块号
	//	_In_ Block* block 指向块的地址
	//Return Value:
	//	true 执行成功
	//	false 执行失败
	bool writeBlock(_In_ PhysicalAddress pa, _In_ DWORD blockId, _In_ Block* block);

	bool format(DWORD partitions);

	bool load(TCHAR* szDiskFileName, BYTE bIdDisk);

	bool save();

	bool create(__int64 size);

	//Description:
	//	在当前路径所在分区申请一个块
	//Paramter: 
	//	_In_ DWORD previousBlock 前一个块的索引 若为0xffffffff，则申请新块
	//Return Value:
	//	0 申请失败
	//	>0 申请成功 返回FAT项索引
	DWORD mallocBlock(_In_ DWORD previousBlock = 0xffffffff);

	//Description:
	//	释放当前路径所在分区一个块,及其后续块
	//Paramter: 
	//	_In_ DWORD blockId 将要释放的块号
	//Return Value:
	//	0 申请失败
	//	>0 申请成功 返回FAT项索引
	bool freeBlock(_In_ DWORD blockId);
private:
	std::vector<Partition*> _PartitionList;
	//std::tstring diskFileName;
};

