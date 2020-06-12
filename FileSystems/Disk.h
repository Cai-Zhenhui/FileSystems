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
#pragma pack(0)

#define DiskPath TEXT(".\\Disk\\")
#define ExpandName TEXT(".disk")

//	�����ཫһ�������ļ����ص��ڴ��У�ά������ڴ���ģ�����
class Disk
{
public:
	DiskHeader dh;
	DWORD blocks;
	FILE* pFile;
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

	bool load(TCHAR szDiskFileName);

	bool save();

	bool create(__int64 size);
private:
	std::vector<Partition> _PartitionList;
	//std::tstring diskFileName;
};

