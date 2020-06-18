#include "stdafx.h"
#include "Partition.h"

Partition::Partition()
{
	Println("Partition����%0X", this);
}

Partition::~Partition()
{
	Println("Partition����%0X", this);
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
	FAT[0] = EOB;//������Ŀ¼����
	return false;
}

auto Partition::getFATItem()
{
	DWORD i = 1;//��1��Ĭ���Ǹ�Ŀ¼�����Դӵڶ��ʼ
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

	FAT[newItem] = EOB;//��������Ŀ�д��FAT��
	if (previousBlock != 0xffffffff) {
		//��Ϊ֮ǰ�Ŀ���չ����  ��Ҫ�޸�֮ǰ��FAT��
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
