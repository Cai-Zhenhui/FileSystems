//cmdÃüÁîĞĞ½Ó¿Ú
#include "stdafx.h"
#include <iostream>
#include "DiskManagement.h"
extern std::vector<User> listUser;
extern int a;
int main() {
	DiskManagement dm;
	dm.generateDisk(1024*20);
	return 0;
}