#include "stdafx.h"
#include "Interface.h"
#include <iostream> 
const char blackCharList[] = {
	'\\','/',':','*','?','\"','<','>','|'
};
bool isExist(char* szFileName) {
	bool isExist = false;
	for (int i = 0; i < currentsFCB.size(); ++i) {
		if (strcmp(currentIIM[i].fileName, szFileName) == 0) {
			isExist = true;
			break;
		}
	}
	return isExist;
}

bool checkFileName(char* szFileName)
{
	int i = 0;
	for (i = 0; szFileName[i] != '\0' && i < MAXFileNameLength; ++i) {
		if (szFileName[i]!= blackCharList[0] &&
			szFileName[i] != blackCharList[1] &&
			szFileName[i] != blackCharList[2] &&
			szFileName[i] != blackCharList[3] &&
			szFileName[i] != blackCharList[4] &&
			szFileName[i] != blackCharList[5] &&
			szFileName[i] != blackCharList[6] &&
			szFileName[i] != blackCharList[7] &&
			szFileName[i] != blackCharList[8] &&
			szFileName[i] >= 0x21 && szFileName[i] <= 0x7e) {

		}
		else {
			return false;
		}
	}
	if (i== MAXFileNameLength && szFileName[MAXFileNameLength - 1] != '\0') {
		//���ȳ�������
		return false;
	}
	return true;
}

bool fCreate(char* szFileName)
{
	if (!diskManagement.isLoad()) {
		return false;
	}
	//����ļ����Ƿ�Ϸ�
	if (!checkFileName(szFileName)) {
		return false;
	}
	if (isExist(szFileName)) {
		return false;
	}

	FileControlBlock fcb(szFileName);
	fcb.sFCB.dwIndexFAT = diskManagement.mallocBlock();
	if (fcb.sFCB.dwIndexFAT == 0) {
		return false;//����ʧ�� �����ʧ��
	}
	fcb.sFCB.dwFileLength = 1;
	fcb.sFCB.fileType = FILETYPE::FT_FILE;
	fcb.sFCB.wLinkTimes = 1;

	//�����ļ��ڴ������ṹд�뵱ǰĿ¼
	IndexItemMem iim(fcb);
	SubFileControlBlock sfcb(fcb);
	currentIIM.push_back(iim);
	currentsFCB.push_back(sfcb);
	//����
	diskManagement.insertFCB();
	return true;
}

bool fDelete(char* szFileName)
{
	if (!diskManagement.isLoad()) {
		return false;
	}
	//����ļ����Ƿ�Ϸ�
	if (!checkFileName(szFileName)) {
		return false;
	}

	//Ѱ��Ҫ�������ļ�
	int i = 0;
	for (; i < currentIIM.size(); ++i) {
		if (currentIIM[i] == szFileName) {
			break;
		}
	}
	if (i >= currentIIM.size()) {
		return false;//Ŀ���ļ�������
	}

	//�ͷſ�
	diskManagement.freeBlock(currentsFCB[i].dwIndexFAT);

	char szFileNameOld[MAXFileNameLength] = { 0 };
	strcpy(szFileNameOld, currentIIM[i].fileName);//�ݴ�Ҫɾ�����ļ���

	//ɾ�����ļ��ڵ�ǰĿ¼�еĵ��ļ����ƿ�
	currentIIM[i].fileName[0] = DELETE_FLAG;
	currentsFCB[i].dwFileLength = 0;

	diskManagement.updateFCB(i, szFileNameOld);

	//���¶�ȡĿ¼��
	diskManagement.readFCBItem(currentsFCB[0].dwIndexFAT);
	return true;
}

Block blockBuffer;
std::vector<IndexItemMem> openListIIM;
std::vector<SubFileControlBlock> openListSFCB;
bool fOpen(char* szFileName, BYTE bFileMode)
{
	if (!diskManagement.isLoad()) {
		return false;
	}
	//����ļ����Ƿ�Ϸ�
	if (!checkFileName(szFileName)) {
		return false;
	}

	//Ѱ��Ҫ�������ļ�
	int i = 0;
	for (; i < currentIIM.size(); ++i) {
		if (currentIIM[i] == szFileName) {
			break;
		}
	}
	if (i >= currentIIM.size()) {
		return false;//Ŀ���ļ�������
	}

	switch (bFileMode)
	{
	case FM_Read: {
		//ֻ��ģʽ�¿��Ա�������̴�
		break;
	}
	case FM_Write: {
		//ֻдģʽ��ֻ�ܱ���һ��
		break;
	}
	case FM_Read | FM_Write: {
		break;
	}
	case FM_Read | FM_Binary: {
		break;
	}
	case FM_Write | FM_Binary: {
		break;
	}
	case FM_Read | FM_Write|FM_Binary: {
		break;
	}
	}

	IndexItemMem iim;
	iim.fileId = openListIIM.size() + 1;
	iim.physicalAddress.bIdDisk = diskManagement.currentDisk->bIdDisk;
	iim.physicalAddress.bIdPartition = diskManagement.currentDisk->currentPartition->ph.bIdPartition;
	iim = szFileName;
	iim.bFileMode = bFileMode;
	openListIIM.push_back(iim);
	openListSFCB.push_back(currentsFCB[i]);

	return true;
}

bool dIntoSub(char* szFileName) {
	if (!diskManagement.isLoad()) {
		return false;
	}
	//����ļ����Ƿ�Ϸ�
	if (!checkFileName(szFileName)) {
		return false;
	}

	//Ѱ��Ҫ�������Ŀ¼���ļ���
	int i = 0;
	for ( ; i < currentIIM.size(); ++i) {
		if (currentIIM[i] == szFileName) {
			break;
		}
	}
	if (i >= currentIIM.size()) {
		return false;//��Ŀ¼������
	}else if (i == 0) {
		//��Ŀ¼
		return true;
	}
	else if (i == 1) {
		if (diskManagement.currentDirectory.size() == 1) {
			//��Ŀ¼
			return false;
		}
		//�ϼ�Ŀ¼
		diskManagement.currentDirectory.pop_back();
	}
	else {
		//��Ŀ¼
		diskManagement.currentDirectory.push_back(szFileName);
	}
	//��ָ�������Ŀ¼
	diskManagement.readFCBItem(currentsFCB[i].dwIndexFAT);
	return true;
}

bool dCreate(char* szFileName)
{
	if (!diskManagement.isLoad()) {
		return false;
	}
	//����ļ����Ƿ�Ϸ�
	if (!checkFileName(szFileName)) {
		return false;
	}
	if (isExist(szFileName)) {
		return false;
	}

	//�������ļ��е��ļ����ƿ�
	FileControlBlock fcb(szFileName);
	fcb.sFCB.dwIndexFAT = diskManagement.mallocBlock();
	if (fcb.sFCB.dwIndexFAT == 0) {
		return false;//����ʧ�� �����ʧ��
	}
	fcb.sFCB.dwFileLength = BlockSize;
	fcb.sFCB.fileType = FILETYPE::FT_DIRECTORY;
	fcb.sFCB.wLinkTimes = 1;

	//�����ļ��ڴ������ṹд�뵱ǰĿ¼
	IndexItemMem iim(fcb);
	SubFileControlBlock sfcb(fcb);
	currentIIM.push_back(iim);
	currentsFCB.push_back(sfcb);
	//����
	diskManagement.insertFCB();

	//���ö�ȡblock  ����֮������д��
	Block block;
	//������д�� . Ŀ¼��
	FileControlBlock fcb_(".");
	fcb_.sFCB.dwFileLength = BlockSize;
	fcb_.sFCB.fileType = FILETYPE::FT_DIRECTORY;
	fcb_.sFCB.dwIndexFAT = fcb.sFCB.dwIndexFAT;//ָ��ǰĿ¼
	block.write(&fcb_, sizeof(fcb_));
	
	//������д�� .. Ŀ¼��
	FileControlBlock fcb__("..");
	fcb__.sFCB.dwFileLength = BlockSize;
	fcb__.sFCB.fileType = FILETYPE::FT_DIRECTORY;
	fcb__.sFCB.dwIndexFAT = currentsFCB[0].dwIndexFAT;//ָ���ϼ�Ŀ¼(������ϼ�Ŀ¼ ָ��������Ŀ¼��Ϊ��ǰĿ¼)
	block.write(&fcb__, sizeof(fcb__));
	
	PhysicalAddress pa;
	pa.bIdDisk = diskManagement.currentDisk->bIdDisk;
	pa.bIdPartition = diskManagement.currentDisk->currentPartition->bIdPartition;
	diskManagement.writeBlock(pa, fcb.sFCB.dwIndexFAT, &block);//�Ѹ�Ŀ¼��Ӧ�Ŀ�д��

	return true;
}

//�ڲ�����
bool _dDelete(char* szFileName, bool isDeleteDir, bool isRecursive) {
	if (!dIntoSub(szFileName)) {
		return false;//��Ŀ¼������
	}
	//�Ѿ�������Ŀ¼

	if (isRecursive) {//ѡ���Ƿ����µݹ�ɾ��
		for (int i = 0; i < currentIIM.size();) {
			if (currentsFCB[i].fileType == FILETYPE::FT_DIRECTORY && i >= 2) {
				dDelete(currentIIM[i].fileName, isDeleteDir, isRecursive);//�����ᵼ���б����仯
				if (isDeleteDir) {
					//ɾ����Ŀ¼ (��Ŀ¼�µ��ļ��ѱ�ɾ��)
					//�ͷſ�
					diskManagement.freeBlock(currentsFCB[i].dwIndexFAT);

					char szFileNameOld[MAXFileNameLength] = { 0 };
					strcpy(szFileNameOld, currentIIM[i].fileName);//�ݴ�Ҫɾ�����ļ���

					//ɾ�����ļ��ڵ�ǰĿ¼�еĵ��ļ����ƿ�
					currentIIM[i].fileName[0] = DELETE_FLAG;
					currentsFCB[i].dwFileLength = 0;

					diskManagement.updateFCB(i, szFileNameOld);

					//���¶�ȡĿ¼��
					diskManagement.readFCBItem(currentsFCB[0].dwIndexFAT);
				}
			}
			else {
				++i;
			}
		}
	}

	//����ɾ���ļ�����
	for (int i = 0; i < currentIIM.size();) {
		if (currentsFCB[i].fileType == FILETYPE::FT_FILE) {
			fDelete(currentIIM[i].fileName);//�����ᵼ���б����仯
		}
		else {
			++i;
		}
	}
	//����Ŀ¼�ļ�ɾ����� �����ϼ�Ŀ¼
	dIntoSub(currentIIM[1].fileName);
	return true;
}

bool dDelete(char* szFileName, bool isDeleteDir, bool isRecursive) {
	//ɾ����Ŀ¼�µ��ļ�
	_dDelete(szFileName, true, true);

	if (isDeleteDir && isRecursive) {
		//��ѡ���˵ݹ�ɾ�� ����ɾ����Ŀ¼������� ɾ��ѡ���Ŀ¼
		for (int i = 0; i < currentIIM.size(); ++i) {
			if (currentIIM[i] == szFileName && currentsFCB[i].fileType == FILETYPE::FT_DIRECTORY) {
				//ɾ����Ŀ¼ (��Ŀ¼�µ��ļ��ѱ�ɾ��)
				//�ͷſ�
				diskManagement.freeBlock(currentsFCB[i].dwIndexFAT);

				char szFileNameOld[MAXFileNameLength] = { 0 };
				strcpy(szFileNameOld, currentIIM[i].fileName);//�ݴ�Ҫɾ�����ļ���

				//ɾ�����ļ��ڵ�ǰĿ¼�еĵ��ļ����ƿ�
				currentIIM[i].fileName[0] = DELETE_FLAG;
				currentsFCB[i].dwFileLength = 0;

				diskManagement.updateFCB(i, szFileNameOld);

				//���¶�ȡĿ¼��
				diskManagement.readFCBItem(currentsFCB[0].dwIndexFAT);
				break;
			}
		}
	}
	return true;
}

void showCurrentDirectory()
{
	printf("\\");
	for (int i = 0; i < diskManagement.currentDirectory.size(); ++i) {
		printf("%s\\", diskManagement.currentDirectory[i].c_str());
	}
}

void showDir()
{
	if (!diskManagement.isLoad()) {
		return ;
	}
	//��ʾ��ʽ:
	//	ʱ��					����		����
	//	mm/dd/yyyy hh:mm:ss <DIR>	28W
	
	char buffer[26 + MAXFileNameLength];
	//����Ҫ���Ե�Ŀ¼�ļ�
	for (int i=0; i < currentsFCB.size(); ++i) {
		currentsFCB[i].timeCreate.sprint(buffer);
		if (currentsFCB[i].fileType == FILETYPE::FT_DIRECTORY) {
			sprintf(buffer + 19, " <DIR> %s\n", currentIIM[i].fileName);
		}
		else {
			sprintf(buffer + 19, "       %s\n", currentIIM[i].fileName);
		}
		printf("%s", buffer);
	}
}

//�ڲ�����
void _showDirTree(char* szFileName,int length) {
	if (!dIntoSub(szFileName)) {
		return;//��Ŀ¼������
	}

	length -= 1;
	char* buffer = new char[length + 2 + MAXFileNameLength];
	memset(buffer, 0x20, length);
	memset(buffer + length, 0, 1 + MAXFileNameLength);
	//�� ��
	//|

	//����Ҫ��ʾ��Ŀ¼�ļ�
	for (int i = 0; i < currentsFCB.size(); ++i ) {
		if (currentsFCB[i].fileType == FILETYPE::FT_DIRECTORY && i >= 2) {
			sprintf(buffer + length, "����%s\n", currentIIM[i].fileName);
			printf("%s", buffer);
			_showDirTree(currentIIM[i].fileName, strlen(currentIIM[i].fileName)+length+2);//�����ᵼ���б����仯
		}
		else if(currentsFCB[i].fileType==FILETYPE::FT_FILE){
			sprintf(buffer + length, "| %s\n", currentIIM[i].fileName);
			printf("%s", buffer);
		}
	}

	delete[] buffer;
	dIntoSub(currentIIM[1].fileName);
}
void showDirTree()
{
	if (!diskManagement.isLoad()) {
		return;
	}
	auto it = (diskManagement.currentDirectory.end() - 1);

	printf("%s\n", (*it).c_str());

	_showDirTree((char*)(*it).c_str(),
		(*it).length());
	//��ʾ��ʽ:
/*
.
| 123.txt
����111
  | 556.txt
  ����111
  | 123.doc
  ����subdir
	 | 556.txt
	 | 123.doc
	*/
}
