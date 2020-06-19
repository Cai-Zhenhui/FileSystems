#include "stdafx.h"
#include "DiskManagement.h"

DiskManagement::DiskManagement()
{
    Println("DiskManagement构造%0X", this);
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
    //找到存放当前目录的最后一个数据区 插入新的目录项
    DWORD fat = currentsFCB[0].dwIndexFAT;//当前目录的第一个FAT项
    while (fat != EOB) {
        DWORD newFat = currentDisk->currentPartition->FAT[fat];
        if (newFat != EOB) {
            fat = newFat;
        }
        else {
            break;
        }
    }

    //判断是否有剩余空间
    if (currentsFCB.size() % MAXItem == 0) {
        //已满 需要扩展
        fat = this->mallocBlock(fat);
        if (fat <= 0) {
            //扩展块失败
            return false;
        }
    }

    //读取fat指向的块
    PhysicalAddress pa;
    pa.bIdDisk = currentDisk->bIdDisk;
    pa.bIdPartition = currentDisk->currentPartition->ph.bIdPartition;
    readBlock(pa, fat, &blockBuffer);

    //在块后边追加新的FCB (此时以确保当前块可以容纳新的一条FCB)
    for (int i = 0; i < MAXItem; ++i) {
        FileControlBlock fcb;
        blockBuffer.read(&fcb, sizeof(FileControlBlock));

        if (!fcb.isValid()) {
            //找到空位
            blockBuffer.setPos(-SIZE_FCB, SEEK_CUR);//回退游标
            //写入新FCB
            FileControlBlock fcb(*(currentIIM.end() - 1), *(currentsFCB.end() - 1));
            blockBuffer.write(&fcb, sizeof(FileControlBlock));
            break;
        }
    }
    //把整个块写回
    writeBlock(pa, fat, &blockBuffer);
    return true;

}

bool DiskManagement::updateFCB(int indexTarget,char *szFileNameOld)
{
    DWORD fat = currentsFCB[0].dwIndexFAT;//当前目录的第一个FAT项
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
                //找到将要修改的文件控制块
                blockBuffer.setPos(-SIZE_FCB, SEEK_CUR);//回退游标
                //写入新FCB
                FileControlBlock fcb(currentIIM[indexTarget], currentsFCB[indexTarget]);
                blockBuffer.write(&fcb, sizeof(FileControlBlock));
                
                //把整个块写回
                writeBlock(pa, fat, &blockBuffer);

                fat = EOB;//设置跳出外循环
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

    //读取目录项
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
                //break;不能直接跳出循环 因为目录项因为删除操作会导致不连续存储
                continue;
            }

            //创建内存 索引
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
        Println("没有磁盘文件或者错误. GetLastError:%d", GetLastError());
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
                Println("打开文件失败！GetLastError:%d", GetLastError());
            }
            DiskHeader dh;
            fread(&dh, sizeof(dh), 1, pFile);
            fclose(pFile);
            if (!dh.isValid()) {
                continue;
            }
            Disk* disk = new Disk;//由DiskManagement释放
            disk->load((TCHAR*)file.c_str(), indexDisk);
            ++indexDisk;
            _DiskList.push_back(disk);
        } while (FindNextFile(handle, &wfd));

        currentDisk = _DiskList[0];
    }
    FindClose(handle);
}
