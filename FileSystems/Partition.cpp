#include "stdafx.h"
#include "Partition.h"

Partition::Partition()
{
	Println("Partition构造%0X", this);
}

Partition::~Partition()
{
	Println("Partition析构%0X", this);
	for (int i = 0; i < BlockList.size(); ++i) {
		delete BlockList[i];
	}
}

bool Partition::readBlock(DWORD blockId, Block* block)
{
	BlockList[blockId]->copy(block);
	return true;
}

bool Partition::writeBlock(DWORD blockId, Block* block)
{
	block->copy(BlockList[blockId]);
	return true;
}

bool Partition::format()
{
	FAT.clear();
	for (DWORD i = 0; i < ph.dwBlocks; ++i) {
		FAT.push_back(DWORD(0));
	}
	FAT[0] = EOB;//分区根目录结束
	for (int i = 0; i < BlockList.size(); ++i) {
		BlockList[i]->clear();
	}
	return true;
}

auto Partition::getFATItem()
{
	DWORD i = 1;//第1项默认是根目录，所以从第二项开始
	for (; i<FAT.size(); ++i) {
		if (FAT[i] == 0) {
			return i;
		}
	}
	return DWORD(0);
}

DWORD Partition::mallocBlock(DWORD previousBlock)
{
	DWORD newItem = getFATItem();
	if (newItem == 0) {
		return DWORD(0);
	}

	FAT[newItem] = EOB;//将新申请的块写入FAT表
	if (previousBlock != 0xffffffff) {
		//是为之前的块扩展申请  需要修改之前的FAT表
		FAT[previousBlock] = newItem;
	}
	return newItem;
}

bool Partition::freeBlock(DWORD blockId)
{
	DWORD preFAT = blockId;
	DWORD newFAT= blockId;
	do
	{
		newFAT = FAT[preFAT];
		FAT[preFAT] = 0;
		preFAT = newFAT;

	} while (newFAT!=EOB);
	return true;
}
