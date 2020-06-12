#pragma once
#pragma pack(1)
struct PartitionHeader
{
	BYTE bIdPartition;
	DWORD dwBlockStart;//������ʼ���
	DWORD dwBlockEnd;//������ֹ���
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
	//	��ָ�������ַ��ȡһ����
	//Paramter: 
	//	_In_ DWORD blockId ���
	//	_Out_ Block* block ָ���ĵ�ַ
	//Return Value:
	//	true ִ�гɹ�
	//	false ִ��ʧ��
	bool readBlock(_In_ DWORD blockId, _Out_ Block* block);

	//Description:
	//	��ָ��һ����д��ָ�������ַ
	//Paramter: 
	//	_In_ DWORD blockId ���
	//	_In_ Block* block ָ���ĵ�ַ
	//Return Value:
	//	true ִ�гɹ�
	//	false ִ��ʧ��
	bool writeBlock(_In_ DWORD blockId, _In_ Block* block);
private:
	std::vector<Block> _ListBlock;
};

