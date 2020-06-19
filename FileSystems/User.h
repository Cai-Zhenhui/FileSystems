#pragma once
class Users
{
public:
	User currentUser;
	std::vector<User> list;
	DWORD number;
	bool isLogin;


	Users();
	~Users();

	bool login(User & user);

	bool logout(User& user);

	bool create(User& user);

	bool deleteU(User& user);

	bool changePassword(User& user);
	
private:
	FILE* pFile;
};