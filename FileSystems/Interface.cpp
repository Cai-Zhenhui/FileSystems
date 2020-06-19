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
	fcb.sFCB.bOwner = users.currentUser.id;
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
DWORD fOpen(char* szFileName, BYTE bFileMode)
{
	DWORD ret = 0;
	if (!diskManagement.isLoad()) {
		return ret;
	}
	//����ļ����Ƿ�Ϸ�
	if (!checkFileName(szFileName)) {
		return ret;
	}

	//Ѱ��Ҫ�������ļ�
	int i = 0;
	for (; i < currentIIM.size(); ++i) {
		if (currentIIM[i] == szFileName && currentsFCB[i].fileType==FILETYPE::FT_FILE) {
			break;
		}
	}
	if (i >= currentIIM.size()) {
		return ret;//Ŀ���ļ�������
	}

	switch (bFileMode)
	{
	case FM_Read: {
		//ֻ��ģʽ�¿��Ա�������̴�
		break;
	}
	case FM_Write: {
		//ֻдģʽ��ֻ�ܱ���һ��
		//�����ļ��Ƿ񱻴�
		for (int j = 0; j < openListIIM.size(); ++j) {
			if (openListIIM[j] == szFileName) {
				//�Ѵ�
				return ret;
			}
		}
		break;
	}
	case FM_Read | FM_Write: {
		//��дģʽ��ֻ�ܱ���һ��
		for (int j = 0; j < openListIIM.size(); ++j) {
			if (openListIIM[j] == szFileName) {
				//�Ѵ�
				return ret;
			}
		}
		break;
	}
						   /*
	case FM_Read | FM_Binary: {
		break;
	}
	case FM_Write | FM_Binary: {
		break;
	}
	case FM_Read | FM_Write|FM_Binary: {
		break;
	}*/
	}

	IndexItemMem iim;
	iim.nPos = 0;
	iim.fileId = openListIIM.size() + 1;
	iim.physicalAddress.bIdDisk = diskManagement.currentDisk->bIdDisk;
	iim.physicalAddress.bIdPartition = diskManagement.currentDisk->currentPartition->ph.bIdPartition;
	iim = szFileName;
	iim.bFileMode = bFileMode;
	openListIIM.push_back(iim);
	openListSFCB.push_back(currentsFCB[i]);
	
	ret = iim.fileId;
	return ret;
}

DWORD fRead(void* _Buffer, size_t _ElementSize, size_t _ElementCount, DWORD fileId)
{
	size_t size = _ElementSize * _ElementCount;
	DWORD ret = 0;
	if (size <= 0) {
		return ret;
	}
	int i = 0;//��һ��Ŀ¼�е�����
	for (; i < openListIIM.size(); ++i) {
		if (openListIIM[i] == fileId) {
			break;
		}
	}
	if (i >= openListIIM.size()) {
		return ret;//Ŀ���ļ�������
	}

	DWORD prefat = openListSFCB[i].dwIndexFAT + openListIIM[i].nPos / BlockSize;//���㵱ǰ�α����ڿ�
	DWORD fat = prefat;
	DWORD blockOffset;
	DWORD bufferOffset = 0;
	PhysicalAddress pa = openListIIM[i].physicalAddress;
	do
	{
		blockOffset = openListIIM[i].nPos % BlockSize;
		diskManagement.readBlock(pa, fat, &blockBuffer);

		//��ȡ�ļ��α����ڿ�
		blockBuffer.setPos(blockOffset, SEEK_SET);

		if ((blockOffset + size) < BlockSize) {
			//Ҫ��ȡ����û�г�����ǰ�� ֱ��д��
			blockBuffer.read((BYTE*)_Buffer+ bufferOffset, size);
			openListIIM[i].nPos += size;
			bufferOffset += size;

			ret = size;
			break;
		}
		else {
			//Ҫ��ȡ���ݳ�����ǰ��
			//��ȡ����
			size -= (BlockSize - blockOffset);//ʣ��
			blockBuffer.read((BYTE*)_Buffer + bufferOffset, BlockSize - blockOffset);

			openListIIM[i].nPos += (BlockSize - blockOffset);
			bufferOffset += (BlockSize - blockOffset);

			//���Խ�����һ��
			fat = diskManagement.currentDisk->currentPartition->FAT[prefat];

			if (fat != EOB) {
				//������һ��
				prefat = fat;
			}
			else {
				break;
			}
		}
	} while (true);

	openListSFCB[i].timeAccess = Date();
	return ret;
}

DWORD fWrite(void const* _Buffer, size_t _ElementSize, size_t _ElementCount, DWORD fileId)
{
	size_t size = _ElementSize * _ElementCount;
	DWORD ret = 0;
	if (size <= 0) {
		return ret;
	}
	int i = 0;//��һ��Ŀ¼�е�����
	for (; i < openListIIM.size(); ++i) {
		if (openListIIM[i] == fileId) {
			break;
		}
	}
	if (i >= openListIIM.size()) {
		return ret;//Ŀ���ļ�������
	}

	DWORD prefat = openListSFCB[i].dwIndexFAT + openListIIM[i].nPos / BlockSize;//���㵱ǰ�α����ڿ�
	DWORD fat = prefat;
	DWORD blockOffset;
	DWORD bufferOffset = 0;
	PhysicalAddress pa = openListIIM[i].physicalAddress;
	do
	{
		blockOffset = openListIIM[i].nPos % BlockSize;
		diskManagement.readBlock(pa, fat, &blockBuffer);
		
		//��ȡ�ļ��α����ڿ�
		blockBuffer.setPos(blockOffset, SEEK_SET);

		if ((blockOffset + size) < BlockSize) {
			//Ҫд������û�г�����ǰ�� ֱ��д��
			blockBuffer.write((BYTE*)_Buffer + bufferOffset, size);
			openListIIM[i].nPos += size;
			bufferOffset += size;

			//д�ؿ�
			diskManagement.writeBlock(pa, fat, &blockBuffer);
			ret = size;
			break;
		}
		else {
			//Ҫд�����ݳ�����ǰ��
			//д�벿��
			size -= (BlockSize - blockOffset);//ʣ��
			blockBuffer.write((BYTE*)_Buffer + bufferOffset, BlockSize - blockOffset);

			openListIIM[i].nPos += (BlockSize - blockOffset);
			bufferOffset += (BlockSize - blockOffset);

			//д�ؿ�
			diskManagement.writeBlock(pa, fat, &blockBuffer);

			//���Խ�����һ��
			fat = diskManagement.currentDisk->currentPartition->FAT[prefat];

			if (fat != EOB) {
				//������һ��
				prefat = fat;
			}
			else {
				//�ļ�û����һ���� ��Ҫ����
				fat = diskManagement.mallocBlock(prefat);
				if (fat == 0) {
					return ret;
				}
				prefat = fat;
			}
		}
	} while (true);
	openListSFCB[i].dwFileLength += ret;
	openListSFCB[i].timeModify = Date();
	openListSFCB[i].timeAccess = openListSFCB[i].timeModify;
	return ret;
}

bool fClose(DWORD fileId)
{
	int i = 0;//��һ��Ŀ¼�е�����
	for (; i < openListIIM.size(); ++i) {
		if (openListIIM[i] == fileId) {
			break;
		}
	}
	if (i >= openListIIM.size()) {
		return false;//Ŀ���ļ�������
	}

	int j = 0;//�ڵ�ǰĿ¼�µ�����
	for (; j < currentIIM.size(); ++j) {
		if (currentIIM[j] == openListIIM[i].fileName) {
			currentIIM[j] = openListIIM[i];
			currentsFCB[j] = openListSFCB[i];
			break;
		}
	}
	if (j >= currentIIM.size()) {
		return false;//Ŀ���ļ�������
	}

	char szFileNameOld[MAXFileNameLength] = { 0 };
	strcpy(szFileNameOld, openListIIM[i].fileName);//�ݴ�Ҫɾ�����ļ���

	diskManagement.updateFCB(j, szFileNameOld);
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
		else if(users.currentUser==(*(diskManagement.currentDirectory.end()-1))){
			//��ǰĿ¼�Ѿ��Ǹ��û��ĸ�Ŀ¼
			return false;
		}
		//�ϼ�Ŀ¼
		diskManagement.currentDirectory.pop_back();
	}
	else {
		//��Ŀ¼
		if (users.currentUser.id == 0) {//root�û�����ֱ�ӽ���
			diskManagement.currentDirectory.push_back(szFileName);
		}
		else {
			if (currentsFCB[i].bOwner != users.currentUser.id) {
				//Ҫ�����Ŀ¼���Ǹ��û���Ŀ¼
				return false;
			}
			diskManagement.currentDirectory.push_back(szFileName);
		}
		
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
	fcb.sFCB.bOwner = users.currentUser.id;
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
	fcb_.sFCB.bOwner = users.currentUser.id;
	fcb_.sFCB.fileType = FILETYPE::FT_DIRECTORY;
	fcb_.sFCB.dwIndexFAT = fcb.sFCB.dwIndexFAT;//ָ��ǰĿ¼
	block.write(&fcb_, sizeof(fcb_));
	
	//������д�� .. Ŀ¼��
	FileControlBlock fcb__("..");
	fcb__.sFCB.dwFileLength = BlockSize;
	fcb__.sFCB.bOwner = users.currentUser.id;
	fcb__.sFCB.fileType = FILETYPE::FT_DIRECTORY;
	fcb__.sFCB.dwIndexFAT = currentsFCB[0].dwIndexFAT;//ָ���ϼ�Ŀ¼(������ϼ�Ŀ¼ ָ��������Ŀ¼��Ϊ��ǰĿ¼)
	block.write(&fcb__, sizeof(fcb__));
	
	PhysicalAddress pa;
	pa.bIdDisk = diskManagement.currentDisk->bIdDisk;
	pa.bIdPartition = diskManagement.currentDisk->currentPartition->bIdPartition;
	diskManagement.writeBlock(pa, fcb.sFCB.dwIndexFAT, &block);//�Ѹ�Ŀ¼��Ӧ�Ŀ�д��

	return true;
}

bool dCreate(User& user)
{
	char* szFileName = user.username;
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
	fcb.sFCB.bOwner = user.id;
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
	fcb_.sFCB.bOwner = user.id;
	fcb_.sFCB.fileType = FILETYPE::FT_DIRECTORY;
	fcb_.sFCB.dwIndexFAT = fcb.sFCB.dwIndexFAT;//ָ��ǰĿ¼
	block.write(&fcb_, sizeof(fcb_));

	//������д�� .. Ŀ¼��
	FileControlBlock fcb__("..");
	fcb__.sFCB.dwFileLength = BlockSize;
	fcb__.sFCB.bOwner = user.id;
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
	printf(">");
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
