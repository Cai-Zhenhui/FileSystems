#include "stdafx.h"
#include "User.h"
#include "Interface.h"

Users::Users()
{
	pFile = fopen("user", "rb+");
	fread(&number, sizeof(number), 1, pFile);
	for (int i = 0; i < number; ++i) {
		User user;
		fread(&user, sizeof(User), 1, pFile);
		list.push_back(user);
	}
	isLogin = false;
}

Users::~Users()
{
	fseek(pFile, 0, SEEK_SET);
	fwrite(&number, sizeof(number), 1, pFile);
	for (int i = 0; i < number; ++i) {
		fwrite(&list[i], sizeof(User), 1, pFile);
	}
	fclose(pFile);
}

bool Users::login(User& user)
{
	for (int i = 0; i < list.size(); ++i) {
		if (list[i] == user) {
			this->currentUser = user;
			isLogin = true;
			return true;
		}
	}
	isLogin = false;
	return false;
}

bool Users::logout(User& user)
{
	isLogin = false;
	return true;
}

bool Users::create(User& user)
{
	user.id = list.size();
	list.push_back(user);
	++number;
	dCreate(user);
	return true;
}

bool Users::deleteU(User& user)
{
	if (user == currentUser) {
		return false;
	}
	auto it=list.begin();
	for (; it !=list.end(); ++it) {
		if ((*it)==user) {
			--number;
			list.erase(it);
			return true;
		}
	}
	return false;
}

bool Users::changePassword(User& user)
{
	if (user == currentUser) {
		for (int i = 0; i < list.size(); ++i) {
			if (list[i] == user) {
				this->currentUser = user;
				return true;
			}
		}
	}
	return false;
}
