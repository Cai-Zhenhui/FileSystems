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
		//长度超过限制
		return false;
	}
	return true;
}

bool fCreate(char* szFileName)
{
	if (!diskManagement.isLoad()) {
		return false;
	}
	//检查文件名是否合法
	if (!checkFileName(szFileName)) {
		return false;
	}
	if (isExist(szFileName)) {
		return false;
	}

	FileControlBlock fcb(szFileName);
	fcb.sFCB.dwIndexFAT = diskManagement.mallocBlock();
	if (fcb.sFCB.dwIndexFAT == 0) {
		return false;//创建失败 申请块失败
	}
	fcb.sFCB.dwFileLength = 1;
	fcb.sFCB.bOwner = users.currentUser.id;
	fcb.sFCB.fileType = FILETYPE::FT_FILE;
	fcb.sFCB.wLinkTimes = 1;

	//将该文件内存索引结构写入当前目录
	IndexItemMem iim(fcb);
	SubFileControlBlock sfcb(fcb);
	currentIIM.push_back(iim);
	currentsFCB.push_back(sfcb);
	//更新
	diskManagement.insertFCB();
	return true;
}

bool fDelete(char* szFileName)
{
	if (!diskManagement.isLoad()) {
		return false;
	}
	//检查文件名是否合法
	if (!checkFileName(szFileName)) {
		return false;
	}

	//寻找要操作的文件
	int i = 0;
	for (; i < currentIIM.size(); ++i) {
		if (currentIIM[i] == szFileName) {
			break;
		}
	}
	if (i >= currentIIM.size()) {
		return false;//目标文件不存在
	}

	//释放块
	diskManagement.freeBlock(currentsFCB[i].dwIndexFAT);

	char szFileNameOld[MAXFileNameLength] = { 0 };
	strcpy(szFileNameOld, currentIIM[i].fileName);//暂存要删除的文件名

	//删除该文件在当前目录中的的文件控制块
	currentIIM[i].fileName[0] = DELETE_FLAG;
	currentsFCB[i].dwFileLength = 0;

	diskManagement.updateFCB(i, szFileNameOld);

	//重新读取目录项
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
	//检查文件名是否合法
	if (!checkFileName(szFileName)) {
		return ret;
	}

	//寻找要操作的文件
	int i = 0;
	for (; i < currentIIM.size(); ++i) {
		if (currentIIM[i] == szFileName && currentsFCB[i].fileType==FILETYPE::FT_FILE) {
			break;
		}
	}
	if (i >= currentIIM.size()) {
		return ret;//目标文件不存在
	}

	switch (bFileMode)
	{
	case FM_Read: {
		//只读模式下可以被多个进程打开
		break;
	}
	case FM_Write: {
		//只写模式下只能被打开一个
		//检查该文件是否被打开
		for (int j = 0; j < openListIIM.size(); ++j) {
			if (openListIIM[j] == szFileName) {
				//已打开
				return ret;
			}
		}
		break;
	}
	case FM_Read | FM_Write: {
		//读写模式下只能被打开一个
		for (int j = 0; j < openListIIM.size(); ++j) {
			if (openListIIM[j] == szFileName) {
				//已打开
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

DWORD fSeek(DWORD fileId, long _Offset, int _Origin)
{
	int i = 0;//在一打开目录中的索引
	for (; i < openListIIM.size(); ++i) {
		if (openListIIM[i] == fileId) {
			break;
		}
	}
	if (i >= openListIIM.size()) {
		return -1;//目标文件不存在
	}

	int newPos = 0;
	switch (_Origin)
	{
	case SEEK_SET: {
		newPos = _Offset;
		break;
	}
	case SEEK_CUR: {
		newPos = openListIIM[i].nPos + _Offset;
		break;
	}
	case SEEK_END: {
		newPos = (BlockSize - 1) + _Offset;
		break;
	}
	}

	if (newPos / (BlockSize - 1) != 0) {
		//越界
		return -1;
	}
	return openListIIM[i].nPos = newPos;
}

DWORD fRead(void* _Buffer, size_t _ElementSize, size_t _ElementCount, DWORD fileId)
{
	size_t size = _ElementSize * _ElementCount;
	DWORD ret = 0;
	if (size <= 0) {
		return ret;
	}
	int i = 0;//在一打开目录中的索引
	for (; i < openListIIM.size(); ++i) {
		if (openListIIM[i] == fileId) {
			break;
		}
	}
	if (i >= openListIIM.size()) {
		return ret;//目标文件不存在
	}

	DWORD prefat = openListSFCB[i].dwIndexFAT + openListIIM[i].nPos / BlockSize;//计算当前游标所在块
	DWORD fat = prefat;
	DWORD blockOffset;
	DWORD bufferOffset = 0;
	PhysicalAddress pa = openListIIM[i].physicalAddress;
	do
	{
		blockOffset = openListIIM[i].nPos % BlockSize;
		diskManagement.readBlock(pa, fat, &blockBuffer);

		//读取文件游标所在块
		blockBuffer.setPos(blockOffset, SEEK_SET);

		if ((blockOffset + size) < BlockSize) {
			//要读取内容没有超过当前块 直接写入
			blockBuffer.read((BYTE*)_Buffer+ bufferOffset, size);
			openListIIM[i].nPos += size;
			bufferOffset += size;

			ret = size;
			break;
		}
		else {
			//要读取内容超过当前块
			//读取部分
			size -= (BlockSize - blockOffset);//剩余
			blockBuffer.read((BYTE*)_Buffer + bufferOffset, BlockSize - blockOffset);

			openListIIM[i].nPos += (BlockSize - blockOffset);
			bufferOffset += (BlockSize - blockOffset);

			//尝试进入下一块
			fat = diskManagement.currentDisk->currentPartition->FAT[prefat];

			if (fat != EOB) {
				//进入下一块
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
	int i = 0;//在一打开目录中的索引
	for (; i < openListIIM.size(); ++i) {
		if (openListIIM[i] == fileId) {
			break;
		}
	}
	if (i >= openListIIM.size()) {
		return ret;//目标文件不存在
	}

	DWORD prefat = openListSFCB[i].dwIndexFAT + openListIIM[i].nPos / BlockSize;//计算当前游标所在块
	DWORD fat = prefat;
	DWORD blockOffset;
	DWORD bufferOffset = 0;
	PhysicalAddress pa = openListIIM[i].physicalAddress;
	do
	{
		blockOffset = openListIIM[i].nPos % BlockSize;
		diskManagement.readBlock(pa, fat, &blockBuffer);
		
		//读取文件游标所在块
		blockBuffer.setPos(blockOffset, SEEK_SET);

		if ((blockOffset + size) < BlockSize) {
			//要写入内容没有超过当前块 直接写入
			blockBuffer.write((BYTE*)_Buffer + bufferOffset, size);
			openListIIM[i].nPos += size;
			bufferOffset += size;

			//写回块
			diskManagement.writeBlock(pa, fat, &blockBuffer);
			ret = size;
			break;
		}
		else {
			//要写入内容超过当前块
			//写入部分
			size -= (BlockSize - blockOffset);//剩余
			blockBuffer.write((BYTE*)_Buffer + bufferOffset, BlockSize - blockOffset);

			openListIIM[i].nPos += (BlockSize - blockOffset);
			bufferOffset += (BlockSize - blockOffset);

			//写回块
			diskManagement.writeBlock(pa, fat, &blockBuffer);

			//尝试进入下一块
			fat = diskManagement.currentDisk->currentPartition->FAT[prefat];

			if (fat != EOB) {
				//进入下一块
				prefat = fat;
			}
			else {
				//文件没有下一块了 需要申请
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
	int i = 0;//在一打开目录中的索引
	for (; i < openListIIM.size(); ++i) {
		if (openListIIM[i] == fileId) {
			break;
		}
	}
	if (i >= openListIIM.size()) {
		return false;//目标文件不存在
	}

	int j = 0;//在当前目录下的索引
	for (; j < currentIIM.size(); ++j) {
		if (currentIIM[j] == openListIIM[i].fileName) {
			currentIIM[j] = openListIIM[i];
			currentsFCB[j] = openListSFCB[i];
			break;
		}
	}
	if (j >= currentIIM.size()) {
		return false;//目标文件不存在
	}

	char szFileNameOld[MAXFileNameLength] = { 0 };
	strcpy(szFileNameOld, openListIIM[i].fileName);//暂存要删除的文件名

	diskManagement.updateFCB(j, szFileNameOld);
	openListIIM.erase(openListIIM.begin() + i);
	openListSFCB.erase(openListSFCB.begin() + i);
	return true;
}

bool fSearch(char* szFileName)
{
	//遍历
	for (int i = 0; i < currentsFCB.size(); ++i) {
		if (currentsFCB[i].fileType == FILETYPE::FT_DIRECTORY && i >= 2) {
			dIntoSub(currentIIM[i].fileName);//操作会导致列表发生变化
			fSearch(szFileName);
		}
		else if (currentsFCB[i].fileType == FILETYPE::FT_FILE && currentIIM[i] == szFileName) {
			//找到
			showCurrentDirectory();
			char buffer[26 + MAXFileNameLength];
			currentsFCB[i].timeCreate.sprint(buffer);
			//遍历要稍显的目录文件
			sprintf(buffer + 19, "       %s\n", currentIIM[i].fileName);
			printf("%s", buffer);
		}
	}

	dIntoSub(currentIIM[1].fileName);
	return true;
}

bool fCopy(char* szFileName, char* szTargetPath)
{
	if (!diskManagement.isLoad()) {
		return false;
	}
	IndexItemMem iimOld;
	SubFileControlBlock sfcbOld;

	//遍历要显示的目录文件
	for (int i = 0; i < currentsFCB.size(); ++i) {
		if (currentIIM[i] == szFileName) {
			//找到文件
			iimOld = currentIIM[i];
			sfcbOld = currentsFCB[i];
		}
	}

	return true;
}

bool _dIntoSub(char* szFileName) {
	//寻找要进入的子目录的文件块
	int i = 0;
	for (; i < currentIIM.size(); ++i) {
		if (currentIIM[i] == szFileName) {
			break;
		}
	}
	if (i >= currentIIM.size()) {
		return false;//子目录不存在
	}
	else if (i == 0) {
		//该目录
		return true;
	}
	else if (i == 1) {
		if (diskManagement.currentDirectory.size() == 1) {
			//根目录
			return false;
		}
		else if (users.currentUser == (*(diskManagement.currentDirectory.end() - 1))) {
			//当前目录已经是该用户的根目录
			return false;
		}
		//上级目录
		diskManagement.currentDirectory.pop_back();
	}
	else {
		//子目录
		if (users.currentUser.id == 0) {//root用户可以直接进入
			diskManagement.currentDirectory.push_back(szFileName);
		}
		else {
			if (currentsFCB[i].bOwner != users.currentUser.id) {
				//要进入的目录不是该用户的目录
				return false;
			}
			diskManagement.currentDirectory.push_back(szFileName);
		}

	}
	//从指定块加载目录
	diskManagement.readFCBItem(currentsFCB[i].dwIndexFAT);
}

bool fMove(char* szFileName, char* szTargetPath)
{
	return false;
}

bool fRename(char* szFileName, char* szNewFileName)
{
	if (!diskManagement.isLoad()) {
		return false;
	}

	//遍历要显示的目录文件
	int i = 0;
	for (; i < currentsFCB.size(); ++i) {
		if (currentIIM[i]==szFileName) {
			//找到文件
			break;
		}
	}

	char szFileNameOld[MAXFileNameLength] = { 0 };
	strcpy(szFileNameOld, currentIIM[i].fileName);//暂存文件名

	currentIIM[i] = szNewFileName;

	diskManagement.updateFCB(i, szFileNameOld);

	//重新读取目录项
	diskManagement.readFCBItem(currentsFCB[0].dwIndexFAT);

	return true;
}

bool dIntoSub(char* szFileName) {
	if (!diskManagement.isLoad()) {
		return false;
	}
	//检查文件名是否合法
	if (!checkFileName(szFileName)) {
		//return false;
	}
	std::string strFileName(szFileName);
	do
	{
		size_t pos = strFileName.find_first_of('\\');
		std::string substr = strFileName.substr(0, pos);

		_dIntoSub(&substr[0]);

		if (pos == std::string::npos) {
			//单级
			break;
		}
		strFileName = strFileName.substr(pos + 1);
		if (strFileName == "") {
			//目录结束
			break;
		}
	} while (true);
	
	
	return true;
}

bool dCreate(char* szFileName)
{
	if (!diskManagement.isLoad()) {
		return false;
	}
	//检查文件名是否合法
	if (!checkFileName(szFileName)) {
		return false;
	}
	if (isExist(szFileName)) {
		return false;
	}

	//创建该文件夹的文件控制块
	FileControlBlock fcb(szFileName);
	fcb.sFCB.dwIndexFAT = diskManagement.mallocBlock();
	if (fcb.sFCB.dwIndexFAT == 0) {
		return false;//创建失败 申请块失败
	}
	fcb.sFCB.dwFileLength = BlockSize;
	fcb.sFCB.bOwner = users.currentUser.id;
	fcb.sFCB.fileType = FILETYPE::FT_DIRECTORY;
	fcb.sFCB.wLinkTimes = 1;

	//将该文件内存索引结构写入当前目录
	IndexItemMem iim(fcb);
	SubFileControlBlock sfcb(fcb);
	currentIIM.push_back(iim);
	currentsFCB.push_back(sfcb);
	//更新
	diskManagement.insertFCB();

	//不用读取block  创建之后将数据写入
	Block block;
	//创建并写入 . 目录项
	FileControlBlock fcb_(".");
	fcb_.sFCB.dwFileLength = BlockSize;
	fcb_.sFCB.bOwner = users.currentUser.id;
	fcb_.sFCB.fileType = FILETYPE::FT_DIRECTORY;
	fcb_.sFCB.dwIndexFAT = fcb.sFCB.dwIndexFAT;//指向当前目录
	block.write(&fcb_, sizeof(fcb_));
	
	//创建并写入 .. 目录项
	FileControlBlock fcb__("..");
	fcb__.sFCB.dwFileLength = BlockSize;
	fcb__.sFCB.bOwner = users.currentUser.id;
	fcb__.sFCB.fileType = FILETYPE::FT_DIRECTORY;
	fcb__.sFCB.dwIndexFAT = currentsFCB[0].dwIndexFAT;//指向上级目录(这里的上级目录 指的是以新目录作为当前目录)
	block.write(&fcb__, sizeof(fcb__));
	
	PhysicalAddress pa;
	pa.bIdDisk = diskManagement.currentDisk->bIdDisk;
	pa.bIdPartition = diskManagement.currentDisk->currentPartition->bIdPartition;
	diskManagement.writeBlock(pa, fcb.sFCB.dwIndexFAT, &block);//把该目录对应的块写回

	return true;
}

bool dCreate(User& user)
{
	char* szFileName = user.username;
	if (!diskManagement.isLoad()) {
		return false;
	}
	//检查文件名是否合法
	if (!checkFileName(szFileName)) {
		return false;
	}
	if (isExist(szFileName)) {
		return false;
	}

	//创建该文件夹的文件控制块
	FileControlBlock fcb(szFileName);
	fcb.sFCB.dwIndexFAT = diskManagement.mallocBlock();
	if (fcb.sFCB.dwIndexFAT == 0) {
		return false;//创建失败 申请块失败
	}
	fcb.sFCB.dwFileLength = BlockSize;
	fcb.sFCB.bOwner = user.id;
	fcb.sFCB.fileType = FILETYPE::FT_DIRECTORY;
	fcb.sFCB.wLinkTimes = 1;

	//将该文件内存索引结构写入当前目录
	IndexItemMem iim(fcb);
	SubFileControlBlock sfcb(fcb);
	currentIIM.push_back(iim);
	currentsFCB.push_back(sfcb);
	//更新
	diskManagement.insertFCB();

	//不用读取block  创建之后将数据写入
	Block block;
	//创建并写入 . 目录项
	FileControlBlock fcb_(".");
	fcb_.sFCB.dwFileLength = BlockSize;
	fcb_.sFCB.bOwner = user.id;
	fcb_.sFCB.fileType = FILETYPE::FT_DIRECTORY;
	fcb_.sFCB.dwIndexFAT = fcb.sFCB.dwIndexFAT;//指向当前目录
	block.write(&fcb_, sizeof(fcb_));

	//创建并写入 .. 目录项
	FileControlBlock fcb__("..");
	fcb__.sFCB.dwFileLength = BlockSize;
	fcb__.sFCB.bOwner = user.id;
	fcb__.sFCB.fileType = FILETYPE::FT_DIRECTORY;
	fcb__.sFCB.dwIndexFAT = currentsFCB[0].dwIndexFAT;//指向上级目录(这里的上级目录 指的是以新目录作为当前目录)
	block.write(&fcb__, sizeof(fcb__));

	PhysicalAddress pa;
	pa.bIdDisk = diskManagement.currentDisk->bIdDisk;
	pa.bIdPartition = diskManagement.currentDisk->currentPartition->bIdPartition;
	diskManagement.writeBlock(pa, fcb.sFCB.dwIndexFAT, &block);//把该目录对应的块写回

	return true;
}

//内部递归调用
bool _dDelete(char* szFileName, bool isDeleteDir, bool isRecursive) {
	if (!dIntoSub(szFileName)) {
		return false;//子目录不存在
	}
	//已经进入子目录

	if (isRecursive) {//选择是否向下递归删除
		for (int i = 0; i < currentIIM.size();) {
			if (currentsFCB[i].fileType == FILETYPE::FT_DIRECTORY && i >= 2) {
				dDelete(currentIIM[i].fileName, isDeleteDir, isRecursive);//操作会导致列表发生变化
				if (isDeleteDir) {
					//删除该目录 (该目录下的文件已被删除)
					//释放块
					diskManagement.freeBlock(currentsFCB[i].dwIndexFAT);

					char szFileNameOld[MAXFileNameLength] = { 0 };
					strcpy(szFileNameOld, currentIIM[i].fileName);//暂存要删除的文件名

					//删除该文件在当前目录中的的文件控制块
					currentIIM[i].fileName[0] = DELETE_FLAG;
					currentsFCB[i].dwFileLength = 0;

					diskManagement.updateFCB(i, szFileNameOld);

					//重新读取目录项
					diskManagement.readFCBItem(currentsFCB[0].dwIndexFAT);
				}
			}
			else {
				++i;
			}
		}
	}

	//进行删除文件操作
	for (int i = 0; i < currentIIM.size();) {
		if (currentsFCB[i].fileType == FILETYPE::FT_FILE) {
			fDelete(currentIIM[i].fileName);//操作会导致列表发生变化
		}
		else {
			++i;
		}
	}
	//该子目录文件删除完毕 返回上级目录
	dIntoSub(currentIIM[1].fileName);
	return true;
}

bool dDelete(char* szFileName, bool isDeleteDir, bool isRecursive) {
	//删除该目录下的文件
	_dDelete(szFileName, true, true);

	if (isDeleteDir && isRecursive) {
		//当选择了递归删除 并且删除的目录的情况下 删除选择的目录
		for (int i = 0; i < currentIIM.size(); ++i) {
			if (currentIIM[i] == szFileName && currentsFCB[i].fileType == FILETYPE::FT_DIRECTORY) {
				//删除该目录 (该目录下的文件已被删除)
				//释放块
				diskManagement.freeBlock(currentsFCB[i].dwIndexFAT);

				char szFileNameOld[MAXFileNameLength] = { 0 };
				strcpy(szFileNameOld, currentIIM[i].fileName);//暂存要删除的文件名

				//删除该文件在当前目录中的的文件控制块
				currentIIM[i].fileName[0] = DELETE_FLAG;
				currentsFCB[i].dwFileLength = 0;

				diskManagement.updateFCB(i, szFileNameOld);

				//重新读取目录项
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
	//显示格式:
	//	时间					类型		名称
	//	mm/dd/yyyy hh:mm:ss <DIR>	28W
	
	char buffer[26 + MAXFileNameLength];
	//遍历要稍显的目录文件
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

//内部递归调用
void _showDirTree(char* szFileName,int length) {
	if (!dIntoSub(szFileName)) {
		return;//子目录不存在
	}

	length -= 1;
	char* buffer = new char[length + 2 + MAXFileNameLength];
	memset(buffer, 0x20, length);
	memset(buffer + length, 0, 1 + MAXFileNameLength);
	//└ ─
	//|

	//遍历要显示的目录文件
	for (int i = 0; i < currentsFCB.size(); ++i ) {
		if (currentsFCB[i].fileType == FILETYPE::FT_DIRECTORY && i >= 2) {
			sprintf(buffer + length, "└─%s\n", currentIIM[i].fileName);
			printf("%s", buffer);
			_showDirTree(currentIIM[i].fileName, strlen(currentIIM[i].fileName)+length+2);//操作会导致列表发生变化
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
	//显示格式:
/*
.
| 123.txt
└─111
  | 556.txt
  └─111
  | 123.doc
  └─subdir
	 | 556.txt
	 | 123.doc
	*/
}
