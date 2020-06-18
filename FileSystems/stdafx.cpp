//#include "DiskManagement.h"
#include "stdafx.h"

std::vector<User> listUser;

std::vector<IndexItemMem> currentIIM;
//当前目录下的目录项
std::vector<SubFileControlBlock> currentsFCB;
//磁盘管理
DiskManagement diskManagement;

SubFileControlBlock::SubFileControlBlock() {
	dwFileLength = 0;
}

SubFileControlBlock::SubFileControlBlock(FileControlBlock& obj2)
{
	memcpy(this, &obj2.sFCB, sizeof(SubFileControlBlock));
}

SubFileControlBlock& SubFileControlBlock::operator=(FileControlBlock& obj2)
{
	memcpy(this, &obj2.sFCB, sizeof(struct SubFileControlBlock));
	return *this;
}

IndexItemMem::IndexItemMem()
{
	memset(this, 0, sizeof(*this));
}

IndexItemMem::IndexItemMem(FileControlBlock& obj2)
{
	memcpy(fileName, obj2.fileName, MAXFileNameLength * sizeof(char));
	//strcpy(fileName, obj2.fileName);
}

IndexItemMem& IndexItemMem::operator=(FileControlBlock& obj2)
{
	strcpy(fileName, obj2.fileName);
	return *this;
}

IndexItemMem& IndexItemMem::operator=(char* szfileName2)
{
	memset(this->fileName, 0, MAXFileNameLength);
	strcpy(this->fileName, szfileName2);
}

bool IndexItemMem::operator==(char* fileName2)
{
	return strcmp(fileName, fileName2) == 0;
}

FileControlBlock::FileControlBlock()
{
	memset(fileName, 0, MAXFileNameLength * sizeof(char));
}

FileControlBlock::FileControlBlock(char* fileName2)
{
	memset(this->fileName, 0, MAXFileNameLength);
	strcpy(this->fileName, fileName2);
}

FileControlBlock::FileControlBlock(const char* fileName2)
{
	memset(this->fileName, 0, MAXFileNameLength);
	strcpy(this->fileName, fileName2);
}

FileControlBlock::FileControlBlock(IndexItemMem& iimobj2, SubFileControlBlock& sfcbobj2)
{
	memcpy(fileName, iimobj2.fileName, MAXFileNameLength * sizeof(char));
	//strcpy(fileName, iimobj2.fileName);
	memcpy(&this->sFCB, &sfcbobj2, sizeof(struct SubFileControlBlock));
}

bool FileControlBlock::isValid()
{
	return sFCB.dwFileLength != 0 && fileName[0] != DELETE_FLAG;
}

bool FileControlBlock::operator==(FileControlBlock& obj2)
{
	return strcmp(fileName, obj2.fileName) == 0;
}

bool FileControlBlock::operator==(char* fileName2)
{
	return strcmp(fileName, fileName2) == 0;
}

FileControlBlock& FileControlBlock::operator=(IndexItemMem& obj2)
{
	memcpy(fileName, obj2.fileName, MAXFileNameLength * sizeof(char));
	return *this;
}

FileControlBlock& FileControlBlock::operator=(SubFileControlBlock& obj2)
{
	memcpy(&this->sFCB, &obj2, sizeof(struct SubFileControlBlock));
	return *this;
}
