#pragma once
#include "Disk.h"
extern std::vector<IndexItemMem> currentIIM;
extern std::vector<SubFileControlBlock> currentsFCB;
class DiskManagement
{
public:
	//��ǰĿ¼
	std::vector<std::string> currentDirectory;
	
	//��ǰ�����ַ
	Disk* currentDisk;
	
	//�ڴ滺����
	Block blockBuffer;

	DiskManagement();
	~DiskManagement();

	//Description:
	//	����һ���������
	//Paramter: 
	//	_In_ __int64 size ���̴�С
	//Return Value:
	//	true ִ�гɹ�
	//	false ִ��ʧ��
	bool generateDisk(_In_ __int64 size);

	//Description:
	//	�жϴ����Ƿ���سɹ�
	//Paramter: 
	//	None
	//Return Value:
	//	true �Ѽ���
	//	false û�м���
	bool isLoad();

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
	
	//Description:
	//	����ǰĿ¼���б������һ�������̿�
	//Paramter: 
	//	None
	//Return Value:
	//	false ʧ��
	//	true �ɹ�
	bool insertFCB();

	//Description:
	//	���޸ĵ�FCB���ڿ�����д��
	//Paramter: 
	//	_In_ int indexTarget ��Ҫ�޸ĵ�FCB����
	//	_In_ char* szFileNameOld Ҫ�޸ĵ��ļ���
	//Return Value:
	//	false ʧ��
	//	true �ɹ�
	bool updateFCB(_In_ int indexTarget, _In_ char* szFileNameOld);

	//Description:
	//	��ȡָ����ŵ�Ŀ¼��
	//Paramter: 
	//	_In_ DWORD blockId ��Ҫ�ͷŵĿ��
	//Return Value:
	//	0 ����ʧ��
	//	>0 ����ɹ� ����FAT������
	bool readFCBItem(_In_ DWORD blockId);

	//Description:
	//	��ʽ��ָ������
	//Paramter: 
	//	BYTE bIdDisk;//����id
	//	BYTE bIdPartition;//����id
	//Return Value:
	//	false ʧ��
	//	true �ɹ�
	bool format(BYTE bIdDisk, BYTE bIdPartition);
private:
	std::vector<Disk*> _DiskList;
	
	void loadDisk();
};

