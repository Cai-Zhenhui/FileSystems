#include "stdafx.h"
#include <ctime>
#include "Disk.h"

std::vector<DWORD> keyList;
void readList() {
    WIN32_FIND_DATA wfd;
    std::tstring path(DiskPath);
    path += TEXT("*");
    path += ExpandName;

    HANDLE handle = FindFirstFile(path.c_str(), &wfd);
    if (handle == INVALID_HANDLE_VALUE) {
        Println("Error. GetLastError:%d", GetLastError());
    }
    else {
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
            keyList.push_back(dh.dwKey);

        } while (FindNextFile(handle, &wfd));
    }
    FindClose(handle);

}

inline DWORD generateKey() {
    DWORD tmpKey = (DWORD)time(NULL);
    std::vector<DWORD>::iterator it = keyList.begin();
    bool isExist = false;
    for (; it != keyList.end(); ++it) {
        if (tmpKey == (*it)) {
            isExist = true;
            break;
        }
    }
    if (isExist) {
        tmpKey += 1;
    }
    return tmpKey;
}


Disk::Disk()
{
    Println("Disk构造%0X", this);
    pFile = NULL;
}

Disk::~Disk()
{
    Println("Disk析构%0X", this);
    if (pFile != NULL) {
        save();
        fclose(pFile);
        pFile = NULL;
    }
    for (int i = 0; i < _PartitionList.size(); ++i) {
        delete _PartitionList[i];
    }
}

bool Disk::readBlock(PhysicalAddress pa, DWORD blockId, Block* block)
{
    if (pa.bIdPartition >= _PartitionList.size()) {
        return false;
    }
    return _PartitionList[pa.bIdPartition]->readBlock(blockId, block);
}

bool Disk::writeBlock(PhysicalAddress pa, DWORD blockId, Block* block)
{
    if (pa.bIdPartition >= _PartitionList.size()) {
        return false;
    }
    return _PartitionList[pa.bIdPartition]->writeBlock(blockId, block);
}

bool Disk::format(DWORD bIdPartition)
{
    if (pFile == NULL) {
        return false;
    }
    if (bIdPartition >= _PartitionList.size()) {
        return false;
    }
    return _PartitionList[bIdPartition]->format();
}

bool Disk::load(TCHAR* szDiskFileName, BYTE bIdDisk)
{
    if (pFile != NULL) {
        save();//先保存原有磁盘
    }
    this->bIdDisk = bIdDisk;
    pFile = tfopen(szDiskFileName, TEXT("rb+"));
    if (pFile == NULL) {
        return false;
    }
    //读取磁盘文件头
    fread(&dh, sizeof(dh), 1, pFile);
    
    for (BYTE i = 0; i < dh.dwPartitions; ++i) {
        Partition* partition = new Partition;//由Disk析构时释放

        //读取分区索引项
        fread(&partition->ph, sizeof(partition->ph), 1, pFile);
        partition->bIdPartition = i;
        _PartitionList.push_back(partition);
    }
    for (BYTE i = 0; i < dh.dwPartitions; ++i) {
        Partition* partition = _PartitionList[i];

        //auto tmp = *partition->FAT.end();
        DWORD* fat = new DWORD[partition->ph.dwBlocks];
        //auto fat = new auto(*partition->FAT.end())[partition->ph.dwBlocks];

        //读取fat
        fread(fat, sizeof(DWORD), partition->ph.dwBlocks, pFile);
        //写入fat
        for (DWORD j = 0; j < partition->ph.dwBlocks; ++j) {
            partition->FAT.push_back(fat[j]);
        }

        //DWORD re = partition->ph.dwFATSize * BlockSize - partition->ph.dwBlocks * sizeof(DWORD);
        //fseek(pFile, re, SEEK_CUR);
        
        //读取数据块
        for (DWORD j = 0; j < partition->ph.dwBlocks; ++j) {
            Block* block = new Block;//由Partition析构时释放
            fread(block->buffer, sizeof(BYTE), BlockSize, pFile);
            
            partition->BlockList.push_back(block);
        }
        delete[]fat;
    }

    currentPartition = _PartitionList[0];
    return true;
}

bool Disk::save()
{
    fseek(pFile, 0, SEEK_SET);

    //写入磁盘文件头
    fwrite(&dh, sizeof(dh), 1, pFile);

    //写入分区表
    for (BYTE i = 0; i < dh.dwPartitions; ++i) {
        fwrite(&_PartitionList[i]->ph, sizeof(_PartitionList[i]->ph), 1, pFile);//写入分区表项
    }

    //写入每个分区的数据
    for (BYTE i = 0; i < dh.dwPartitions; ++i) {
        //写入FAT
        for (DWORD j = 0; j < _PartitionList[i]->ph.dwBlocks; ++j) {
            fwrite(&_PartitionList[i]->FAT[j], sizeof(*_PartitionList[i]->FAT.end()), 1, pFile);
        }
        
        //写入剩余块
        for (DWORD j = 0; j < _PartitionList[i]->ph.dwBlocks; ++j) {
            fwrite(_PartitionList[i]->BlockList[j], sizeof(BYTE), BlockSize, pFile);
        }
    }
    return true;
}

bool Disk::create(__int64 size)
{
    if (pFile != NULL) {
        save();//先保存原有磁盘
    }
    readList();
    if (size < BlockSize * 10) {
        return false;
    }
    blocks = size / BlockSize;//块数量

    dh.dwKey = generateKey();
    dh.dwSizeHigh = (DWORD)(size >> 32);
    dh.dwSizeLow = (DWORD)size;
    dh.dwPartitions = 1;

    std::tstring fileName(DiskPath);
    TCHAR tmpbuffer[260];
    tsprintf(tmpbuffer, TEXT("%d"), dh.dwKey);
    fileName += tmpbuffer;
    fileName += ExpandName;

    pFile = tfopen(fileName.c_str(), TEXT("wb"));

    //写入磁盘文件头
    fwrite(&dh, sizeof(dh), 1, pFile);

    //写入磁盘分区表 默认1个分区
    size /= BlockSize;
    DWORD BPP = size / dh.dwPartitions;//Block Per Partition 每分区块
    
    for (BYTE i = 0; i < dh.dwPartitions;++i ) {
        PartitionHeader ph;
        Partition* partition = new Partition;

        ph.bIdPartition = i;
        ph.dwBlocks = BPP;
        //计算fat所占块数 不足一块 上线取整
        ph.dwFATSize = (ph.dwBlocks * sizeof(*partition->FAT.end()) + BlockSize) / BlockSize;
        fwrite(&ph, sizeof(ph), 1, pFile);//写入分区表项

        partition->ph = ph;
        partition->format();

        _PartitionList.push_back(partition);
    }

    //写入数据区
    //BYTE block[BlockSize] = { 0 };
    for (BYTE i = 0; i < dh.dwPartitions;++i ) {

        Println("%d", ftell(pFile));

        //写入FAT
        for (DWORD j = 0; j < _PartitionList[i]->ph.dwBlocks; ++j) {
            fwrite(&_PartitionList[i]->FAT[j], sizeof(*_PartitionList[i]->FAT.end()), 1, pFile);
        }
        Println("%d", ftell(pFile));
        //计算不足一块的剩余大小 字节
        //DWORD re = _PartitionList[i]->ph.dwFATSize * BlockSize -
        //    _PartitionList[i]->ph.dwBlocks * sizeof(*_PartitionList[i]->FAT.end());
        //BYTE tmp = 0;
        //fwrite(block, sizeof(BYTE), re, pFile);
        
        //分配块
        {
            //在第一块中写入. 和 ..目录
            Block* block = new Block;//由Partition析构时释放
            FileControlBlock fcb;

            fcb.fileName[0] = '.';
            fcb.fileName[1] = '\0';
            fcb.sFCB.dwFileLength = BlockSize;
            fcb.sFCB.fileType = FILETYPE::FT_DIRECTORY;
            fcb.sFCB.dwIndexFAT = 0;//指向当前目录(也就是根目录)
            block->write(&fcb, sizeof(fcb));

            fcb.fileName[0] = '.';
            fcb.fileName[1] = '.';
            fcb.fileName[2] = '\0';
            fcb.sFCB.dwFileLength = BlockSize;
            fcb.sFCB.fileType = FILETYPE::FT_DIRECTORY;
            fcb.sFCB.dwIndexFAT = 0;//指向当前目录(也就是根目录)
            block->write(&fcb, sizeof(fcb));
            
            fwrite(block->buffer, sizeof(BYTE), BlockSize, pFile);
            _PartitionList[i]->BlockList.push_back(block);
            Println("%d", ftell(pFile));
            //写入剩余块
            for (DWORD j = 1; j < _PartitionList[i]->ph.dwBlocks; ++j) {
                Block* block = new Block;//由Partition析构时释放
                fwrite(block->buffer, sizeof(BYTE), BlockSize, pFile);
                _PartitionList[i]->BlockList.push_back(block);
            }
        }
    }
    fclose(pFile);
    pFile = NULL;
    Sleep(1000);
    return 0;
}

DWORD Disk::mallocBlock(DWORD previousBlock)
{
    return currentPartition->mallocBlock(previousBlock);
}

bool Disk::freeBlock(DWORD blockId)
{
    return currentPartition->freeBlock(blockId);
}


