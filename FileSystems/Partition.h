#pragma once
#pragma pack(1)
struct PartitionHeader
{
	BYTE bIdPartition;
	DWORD dwBlockStart;//分区起始块号
	DWORD dwBlockEnd;//分区终止块号
};
typedef struct PartitionHeader PartitionHeader;
#pragma pack(0)

class Partition
{
public:
	PartitionHeader ph;
	std::vector<DWORD> FAT;

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
private:
	std::vector<Block> _ListBlock;
};

