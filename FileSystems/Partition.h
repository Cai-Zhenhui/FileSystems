#pragma once
#include "stdafx.h"
#pragma pack(1)
struct PartitionHeader
{
	BYTE bIdPartition;
	DWORD dwBlocks;//����ӵ�п������
	DWORD dwFATSize;//FAT��С ��λ:��
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

	//Description:
	//	�Ը÷������и�ʽ��
	//Paramter: 
	//	NONE
	//Return Value:
	//	true ִ�гɹ�
	//	false ִ��ʧ��
	bool format();

	//Description:
	//	��ȡһ�����õ�FAT��
	//Paramter: 
	//	NONE
	//Return Value:
	//	0 �޿���FAT��
	//	>0 ������
	auto getFATItem();

	//Description:
	//	�ڵ�ǰ·�����ڷ�������һ����
	//Paramter: 
	//	_In_ DWORD previousBlock ǰһ��������� ��Ϊ0xffffffff���������¿�
	//Return Value:
	//	0 ����ʧ��
	//	>0 ����ɹ� ����FAT������
	DWORD mallocBlock(_In_ DWORD previousBlock = 0xffffffff);

	//Description:
	//	�ͷŵ�ǰ·�����ڷ���һ����,���������
	//Paramter: 
	//	_In_ DWORD blockId ��Ҫ�ͷŵĿ��
	//Return Value:
	//	0 ����ʧ��
	//	>0 ����ɹ� ����FAT������
	bool freeBlock(_In_ DWORD blockId);
private:
	
};

