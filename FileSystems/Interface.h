#pragma once
//该文件系统所提供的功能

//Description:
//	检查文件名是否符合要求
//Paramter: 
//	_In_ char* szFileName 待检查文件名
//Return Value:
//	true 执行成功
//	false 执行失败
bool checkFileName(_In_ char* szFileName);

//Description:
//	在当前路径下创建一个文件
//Paramter: 
//	_In_ char* szFileName 要创建的文件名
//Return Value:
//	true 执行成功
//	false 执行失败
bool fCreate(_In_ char* szFileName);

//Description:
//	在当前路径下删除一个文件
//Paramter: 
//	_In_ char* szFileName 要删除的文件名
//Return Value:
//	true 执行成功
//	false 执行失败
bool fDelete(_In_ char* szFileName);

#define FM_Read		0x01
#define FM_Write	0x02
#define FM_Binary	0x04

//Description:
//	打开当前路径下的一个文件,返回文件号
//Paramter: 
//	_In_ char* szFileName 文件名
//	_In_ BYTE bFileMode 打开方式
//Return Value:
//	0	打开失败
//	>=0	打开成功
DWORD fOpen(_In_ char* szFileName, _In_ BYTE bFileMode);

//Description:
//	调整游标
//Paramter: 
//  _In_ DWORD fileId 文件号
//  _In_ long _Offset 偏移
//  _In_ int _Origin = SEEK_SET 起始位置
//Return Value:
//	 执行成功
//	-1 执行失败
DWORD fSeek(_In_ DWORD fileId, _In_ long _Offset, _In_ int _Origin = SEEK_SET);

//Description:
//	读取已打开的文件
//Paramter: 
//	_In_reads_bytes_(_ElementSize* _ElementCount) void * _Buffer 的内容
//  _In_ size_t _ElementSize 元素的大小
//  _In_ size_t _ElementCount 元素的数量
//  _In_ DWORD fileId 文件号
//Return Value:
//	DWORD 读取字节数
DWORD fRead(_Out_writes_bytes_(_ElementSize* _ElementCount) void* _Buffer,
    _In_ size_t _ElementSize,
    _In_ size_t _ElementCount,
    _In_ DWORD fileId);

//Description:
//	打开当前路径下的一个文件
//Paramter: 
//	_In_reads_bytes_(_ElementSize* _ElementCount) void * _Buffer 要写入的内容
//  _In_ size_t _ElementSize 写入元素的大小
//  _In_ size_t _ElementCount 写入元素的数量
//  _In_ DWORD fileId 文件号
//Return Value:
//	DWORD 写入字节数
DWORD fWrite(_In_reads_bytes_(_ElementSize* _ElementCount) void const* _Buffer,
    _In_ size_t _ElementSize,
    _In_ size_t _ElementCount,
    _In_ DWORD fileId);

//Description:
//	关闭当前路径下的一个文件
//Paramter: 
//	_In_ char* szFileName 文件名
//Return Value:
//	true 执行成功
//	false 执行失败
bool fClose(_In_ DWORD fileId);

//Description:
//	搜索文件
//Paramter: 
//	_In_ char* szFileName 文件名
//Return Value:
//	true 执行成功
//	false 执行失败
bool fSearch(_In_ char* szFileName);

//Description:
//	复制文件
//Paramter: 
//	_In_ char* szFileName 文件名
//  _In_ char* szTargetPath 目标路径
//Return Value:
//	true 执行成功
//	false 执行失败
bool fCopy(_In_ char* szFileName, _In_ char* szTargetPath);

//Description:
//	移动文件
//Paramter: 
//	_In_ char* szFileName 文件名
//  _In_ char* szTargetPath 目标路径
//Return Value:
//	true 执行成功
//	false 执行失败
bool fMove(_In_ char* szFileName, _In_ char* szTargetPath);

//Description:
//	重命名
//Paramter: 
//	_In_ char* szFileName 文件名
//Return Value:
//	true 执行成功
//	false 执行失败
bool fRename(_In_ char* szFileName, _In_ char* szNewFileName);

//Description:
//	进入当前目录下的一个子目录
//Paramter: 
//	_In_ char* szFileName 目录名
//Return Value:
//	true 执行成功
//	false 执行失败
bool dIntoSub(_In_ char* szFileName);

//Description:
//	在当前路径下创建一个文件夹
//Paramter: 
//	_In_ char* szFileName 要创建的文件夹名
//Return Value:
//	true 执行成功
//	false 执行失败
bool dCreate(_In_ char* szFileName);

//Description:
//	在当前路径下创建一个文件夹
//Paramter: 
//	_In_ User& user 要创建的用户
//Return Value:
//	true 执行成功
//	false 执行失败
bool dCreate(_In_ User& user);

//Description:
//	在当前路径下删除一个文件夹下的文件
//Paramter: 
//	_In_ char* szFileName 要删除的文件夹名
//	_In_ bool isDeleteDir 是否删除文件夹结构
//	_In_ bool isRecursive 是否递归删除
//Return Value:
//	true 执行成功
//	false 执行失败
bool dDelete(_In_ char* szFileName, _In_ bool isDeleteDir = false, _In_ bool isRecursive = false);

//Description:
//	显示当前路径
//Paramter: 
//	None
//Return Value:
//	None
void showCurrentDirectory();

//Description:
//	显示当前目录下所有子文件夹及其文件
//Paramter: 
//	None
//Return Value:
//	None
void showDir();

//Description:
//	递归显示当前目录下的目录树
//Paramter: 
//	None
//Return Value:
//	None
void showDirTree();