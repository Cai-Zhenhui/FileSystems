#pragma once
#include "Disk.h"
extern std::vector<IndexItemMem> currentIIM;
extern std::vector<SubFileControlBlock> currentsFCB;
class DiskManagement
{
public:
	//当前目录
	std::vector<std::string> currentDirectory;
	
	//当前物理地址
	Disk* currentDisk;
	
	//内存缓冲区
	Block blockBuffer;

	DiskManagement();
	~DiskManagement();

	//Description:
	//	创建一个虚拟磁盘
	//Paramter: 
	//	_In_ __int64 size 磁盘大小
	//Return Value:
	//	true 执行成功
	//	false 执行失败
	bool generateDisk(_In_ __int64 size);

	//Description:
	//	判断磁盘是否加载成功
	//Paramter: 
	//	None
	//Return Value:
	//	true 已加载
	//	false 没有加载
	bool isLoad();

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
	
	//Description:
	//	将当前目录项列表中最后一项插入磁盘块
	//Paramter: 
	//	None
	//Return Value:
	//	false 失败
	//	true 成功
	bool insertFCB();

	//Description:
	//	将修改的FCB所在块重新写入
	//Paramter: 
	//	_In_ int indexTarget 需要修改的FCB索引
	//	_In_ char* szFileNameOld 要修改的文件名
	//Return Value:
	//	false 失败
	//	true 成功
	bool updateFCB(_In_ int indexTarget, _In_ char* szFileNameOld);

	//Description:
	//	读取指定块号的目录项
	//Paramter: 
	//	_In_ DWORD blockId 将要释放的块号
	//Return Value:
	//	0 申请失败
	//	>0 申请成功 返回FAT项索引
	bool readFCBItem(_In_ DWORD blockId);

private:
	std::vector<Disk*> _DiskList;
	
	void loadDisk();
};

