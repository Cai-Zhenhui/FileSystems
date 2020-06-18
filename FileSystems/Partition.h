#pragma once
#include "stdafx.h"
#pragma pack(1)
struct PartitionHeader
{
	BYTE bIdPartition;
	DWORD dwBlocks;//分区拥有块的数量
	DWORD dwFATSize;//FAT大小 单位:块
};
typedef struct PartitionHeader PartitionHeader;
#pragma pack()

#define EOB 0x0fffffff

class Partition
{
public:
	PartitionHeader ph;
	BYTE bIdPartition;
	std::vector<DWORD> FAT;
	std::vector<Block*> BlockList;

	Partition();
	~Partition();


	//Description:
	//	从指定物理地址读取一个块
	//Paramter: 
	//	_In_ DWORD blockId 块号
	//	_Out_ Block* block 指向块的地址
	//Return Value:
	//	true 执行成功
	//	false 执行失败
	bool readBlock(_In_ DWORD blockId, _Out_ Block* block);

	//Description:
	//	从指将一个块写入指定物理地址
	//Paramter: 
	//	_In_ DWORD blockId 块号
	//	_In_ Block* block 指向块的地址
	//Return Value:
	//	true 执行成功
	//	false 执行失败
	bool writeBlock(_In_ DWORD blockId, _In_ Block* block);

	//Description:
	//	对该分区进行格式化
	//Paramter: 
	//	NONE
	//Return Value:
	//	true 执行成功
	//	false 执行失败
	bool format();

	//Description:
	//	获取一个可用的FAT项
	//Paramter: 
	//	NONE
	//Return Value:
	//	0 无可用FAT项
	//	>0 可用项
	auto getFATItem();

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
	
};

