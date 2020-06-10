#pragma once
#include <vector>
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
	bool writeBlock(_In_ PhysicalAddress pa, _In_ Block* block);
private:
	std::vector<Disk> _ListDisk;
	
	void loadDisk();
};

