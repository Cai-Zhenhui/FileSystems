#include "stdafx.h"
#include "DiskManagement.h"

DiskManagement::DiskManagement()
{
}

DiskManagement::~DiskManagement()
{
}

bool DiskManagement::generateDisk(__int64 size)
{
	Disk disk;
	return disk.create(size);
}
