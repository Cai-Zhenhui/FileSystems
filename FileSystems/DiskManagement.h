#pragma once
#include "Disk.h"
class DiskManagement
{
public:
	DiskManagement();
	~DiskManagement();

	//Description:
	//	从指定物理地址读取一个块
	//Paramter: 
	//	_In_ PhysicalAddress pa 物理地址
	//	_In_ DWORD blockId 块号
	//	_Out_ Block* block 指向块的地址
	//Return Value:
	//	true 执行成功
	//	false 执行失败
	bool readBlock(_In_ PhysicalAddress pa,_In_ DWORD blockId, _Out_ Block* block);

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
	//	创建一个虚拟磁盘
	//Paramter: 
	//	_In_ __int64 size 磁盘大小
	//Return Value:
	//	true 执行成功
	//	false 执行失败
	bool generateDisk(_In_ __int64 size);
private:
	std::vector<Disk> _ListDisk;
	
	void loadDisk();
};

