#pragma once
#include "Partition.h"
#pragma pack(1)
struct DiskHeader
{
	WORD wFlag;//DF
	DWORD dwKey;//keyΨһ��ʶ����������
	DWORD dwSizeLow;//���̴�С��32
	DWORD dwSizeHigh;//���̴�С��32
	DWORD dwPartitions;//��������
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

//	�����ཫһ�������ļ����ص��ڴ��У�ά������ڴ���ģ�����
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
	//	��ָ�������ַ��ȡһ����
	//Paramter: 
	//	_In_ PhysicalAddress pa �����ַ
	//	_In_ DWORD blockId ���
	//	_Out_ Block* block ָ���ĵ�ַ
	//Return Value:
	//	true ִ�гɹ�
	//	false ִ��ʧ��
	bool readBlock(_In_ PhysicalAddress pa, _In_ DWORD blockId, _Out_ Block* block);

	//Description:
	//	��ָ��һ����д��ָ�������ַ
	//Paramter: 
	//	_In_ PhysicalAddress pa �����ַ
	//	_In_ DWORD blockId ���
	//	_In_ Block* block ָ���ĵ�ַ
	//Return Value:
	//	true ִ�гɹ�
	//	false ִ��ʧ��
	bool writeBlock(_In_ PhysicalAddress pa, _In_ DWORD blockId, _In_ Block* block);

	bool format(DWORD partitions);

	bool load(TCHAR* szDiskFileName, BYTE bIdDisk);

	bool save();

	bool create(__int64 size);

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
	std::vector<Partition*> _PartitionList;
	//std::tstring diskFileName;
};

