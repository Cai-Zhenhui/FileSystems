#include "stdafx.h"
#include <iostream>
#include "Common.h"
#include "DiskManagement.h"
extern std::vector<User> listUser;
extern int a;
int main() {
	Println("ÖĞÎÄ²âÊÔ:123 %d %s", 321, TEXT("ABC"));
	User user;
	user.id = 20;
	user.username = TEXT("admin");
	user.password = TEXT("123");
	listUser.push_back(user);
	DiskManagement dm;
	return 0;
}