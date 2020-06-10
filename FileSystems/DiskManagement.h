#pragma once
#include <vector>
#include "Disk.h"
class DiskManagement
{
public:
	DiskManagement();
	~DiskManagement();

	//Description:
	//	��ָ�������ַ��ȡһ����
	//Paramter: 
	//	_In_ PhysicalAddress pa �����ַ
	//	_In_ DWORD blockId ���
	//	_Out_ Block* block ָ���ĵ�ַ
	//Return Value:
	//	true ִ�гɹ�
	//	false ִ��ʧ��
	bool readBlock(_In_ PhysicalAddress pa,_In_ DWORD blockId, _Out_ Block* block);

	//Description:
	//	��ָ��һ����д��ָ�������ַ
	//Paramter: 
	//	_In_ PhysicalAddress pa �����ַ
	//	_In_ DWORD blockId ���
	//	_In_ Block* block ָ���ĵ�ַ
	//Return Value:
	//	true ִ�гɹ�
	//	false ִ��ʧ��
	bool writeBlock(_In_ PhysicalAddress pa, _In_ Block* block);
private:
	std::vector<Disk> _ListDisk;
	
	void loadDisk();
};

