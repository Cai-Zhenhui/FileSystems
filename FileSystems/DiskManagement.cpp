#include "stdafx.h"
#include "DiskManagement.h"

DiskManagement::DiskManagement()
{
    Println("DiskManagement����%0X", this);
    loadDisk();
    if (!isLoad()) {
        return;
    }

    this->readFCBItem(0);
}

DiskManagement::~DiskManagement()
{
    for (int i = 0; i < _DiskList.size(); ++i) {
        delete _DiskList[i];
    }
}

bool DiskManagement::generateDisk(__int64 size)
{
	Disk disk;
	return disk.create(size);
}

bool DiskManagement::isLoad()
{
    return _DiskList.size() != 0;
}

bool DiskManagement::readBlock(PhysicalAddress pa, DWORD blockId, Block* block)
{
    if (block == NULL) {
        return false;
    }
    if (pa.bIdDisk >= _DiskList.size()) {
        return false;
    }
    _DiskList[pa.bIdDisk]->readBlock(pa, blockId, block);
    return true;
}

bool DiskManagement::writeBlock(PhysicalAddress pa, DWORD blockId, Block* block)
{
    if (block == NULL) {
        return false;
    }
    if (pa.bIdDisk >= _DiskList.size()) {
        return false;
    }
    _DiskList[pa.bIdDisk]->writeBlock(pa, blockId, block);
    return true;
}

DWORD DiskManagement::mallocBlock(DWORD previousBlock)
{
    if (!isLoad()) {
        return DWORD(0);
    }
    return currentDisk->mallocBlock(previousBlock);
}

bool DiskManagement::freeBlock(DWORD blockId)
{
    if (!isLoad()) {
        return false;
    }
    return currentDisk->freeBlock(blockId);
}

bool DiskManagement::insertFCB()
{
    //�ҵ���ŵ�ǰĿ¼�����һ�������� �����µ�Ŀ¼��

    DWORD fat = currentsFCB[0].dwIndexFAT;//��ǰĿ¼�ĵ�һ��FAT��
    while (fat != EOB) {
        DWORD newFat = currentDisk->currentPartition->FAT[fat];
        if (newFat != EOB) {
            fat = newFat;
        }
        else {
            break;
        }
    }

    //�ж��Ƿ���ʣ��ռ�
    if (currentsFCB.size() % MAXItem == 0) {
        //���� ��Ҫ��չ
        fat = this->mallocBlock(fat);
        if (fat <= 0) {
            //��չ��ʧ��
            return false;
        }
    }

    //��ȡfatָ��Ŀ�
    PhysicalAddress pa;
    pa.bIdDisk = currentDisk->bIdDisk;
    pa.bIdPartition = currentDisk->currentPartition->ph.bIdPartition;
    readBlock(pa, fat, &blockBuffer);

    //�ڿ���׷���µ�FCB (��ʱ��ȷ����ǰ����������µ�һ��FCB)
    for (int i = 0; i < MAXItem; ++i) {
        FileControlBlock fcb;
        blockBuffer.read(&fcb, sizeof(FileControlBlock));

        if (!fcb.isValid()) {
            //�ҵ���λ
            blockBuffer.setPos(-SIZE_FCB, SEEK_CUR);//�����α�
            //д����FCB
            FileControlBlock fcb(*(currentIIM.end() - 1), *(currentsFCB.end() - 1));
            blockBuffer.write(&fcb, sizeof(FileControlBlock));
            break;
        }
    }
    //��������д��
    writeBlock(pa, fat, &blockBuffer);
    return true;

}

bool DiskManagement::updateFCB(int indexTarget,char *szFileNameOld)
{
    DWORD fat = currentsFCB[0].dwIndexFAT;//��ǰĿ¼�ĵ�һ��FAT��
    do
    {
        PhysicalAddress pa;
        pa.bIdDisk = currentDisk->bIdDisk;
        pa.bIdPartition = currentDisk->currentPartition->ph.bIdPartition;
        readBlock(pa, fat, &blockBuffer);
        for (int i = 0; i < MAXItem; ++i) {
            FileControlBlock fcb;
            blockBuffer.read(&fcb, sizeof(FileControlBlock));
            if (fcb== szFileNameOld) {
                //�ҵ���Ҫ�޸ĵ��ļ����ƿ�
                blockBuffer.setPos(-SIZE_FCB, SEEK_CUR);//�����α�
                //д����FCB
                FileControlBlock fcb(currentIIM[indexTarget], currentsFCB[indexTarget]);
                blockBuffer.write(&fcb, sizeof(FileControlBlock));
                
                //��������д��
                writeBlock(pa, fat, &blockBuffer);

                fat = EOB;//����������ѭ��
                break;
            }
        }
        if (fat != EOB) {
            fat = currentDisk->currentPartition->FAT[fat];
        }
        else {
            break;
        }
    } while (true);
    return true;
}

bool DiskManagement::readFCBItem(DWORD blockId)
{
    currentsFCB.clear();
    currentIIM.clear();

    //��ȡĿ¼��
    DWORD fat = currentDisk->currentPartition->FAT[blockId];
    do
    {
        PhysicalAddress pa;
        pa.bIdDisk = currentDisk->bIdDisk;
        pa.bIdPartition = currentDisk->currentPartition->ph.bIdPartition;
        readBlock(pa, blockId, &blockBuffer);
        for (int i = 0; i < MAXItem; ++i) {
            FileControlBlock fcb;
            blockBuffer.read(&fcb, sizeof(FileControlBlock));
            if (!fcb.isValid()) {
                //break;����ֱ������ѭ�� ��ΪĿ¼����Ϊɾ�������ᵼ�²������洢
                continue;
            }

            //�����ڴ� ����
            IndexItemMem iim = fcb;
            SubFileControlBlock sfcb = fcb;

            currentIIM.push_back(iim);
            currentsFCB.push_back(sfcb);
        }
        if (fat != EOB) {
            fat = currentDisk->currentPartition->FAT[fat];
        }
        else {
            break;
        }
    } while (true);
    return true;
}

bool DiskManagement::format(BYTE bIdDisk, BYTE bIdPartition)
{
    if (!isLoad()) {
        return false;
    }
    if (bIdDisk >= _DiskList.size()) {
        return false;
    }
    return _DiskList[bIdDisk]->format(bIdPartition);
}

void DiskManagement::loadDisk()
{
    WIN32_FIND_DATA wfd;
    std::tstring path(DiskPath);
    path += TEXT("*");
    path += ExpandName;

    HANDLE handle = FindFirstFile(path.c_str(), &wfd);
    int indexDisk = 0;
    if (handle == INVALID_HANDLE_VALUE) {
        currentDisk = NULL;
        Println("û�д����ļ����ߴ���. GetLastError:%d", GetLastError());
        FindClose(handle);
        return;
    }
    else {
        currentDirectory.push_back(".");
        do
        {
            Println("%s", wfd.cFileName);
            std::tstring file(DiskPath);
            file += wfd.cFileName;
            FILE* pFile = tfopen(file.c_str(), TEXT("rb"));
            if (pFile == NULL) {
                Println("���ļ�ʧ�ܣ�GetLastError:%d", GetLastError());
            }
            DiskHeader dh;
            fread(&dh, sizeof(dh), 1, pFile);
            fclose(pFile);
            if (!dh.isValid()) {
                continue;
            }
            Disk* disk = new Disk;//��DiskManagement�ͷ�
            disk->load((TCHAR*)file.c_str(), indexDisk);
            ++indexDisk;
            _DiskList.push_back(disk);
        } while (FindNextFile(handle, &wfd));

        currentDisk = _DiskList[0];
    }
    FindClose(handle);
}
