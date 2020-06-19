#pragma once
//���ļ�ϵͳ���ṩ�Ĺ���

//Description:
//	����ļ����Ƿ����Ҫ��
//Paramter: 
//	_In_ char* szFileName ������ļ���
//Return Value:
//	true ִ�гɹ�
//	false ִ��ʧ��
bool checkFileName(_In_ char* szFileName);

//Description:
//	�ڵ�ǰ·���´���һ���ļ�
//Paramter: 
//	_In_ char* szFileName Ҫ�������ļ���
//Return Value:
//	true ִ�гɹ�
//	false ִ��ʧ��
bool fCreate(_In_ char* szFileName);

//Description:
//	�ڵ�ǰ·����ɾ��һ���ļ�
//Paramter: 
//	_In_ char* szFileName Ҫɾ�����ļ���
//Return Value:
//	true ִ�гɹ�
//	false ִ��ʧ��
bool fDelete(_In_ char* szFileName);

#define FM_Read		0x01
#define FM_Write	0x02
#define FM_Binary	0x04

//Description:
//	�򿪵�ǰ·���µ�һ���ļ�,�����ļ���
//Paramter: 
//	_In_ char* szFileName �ļ���
//	_In_ BYTE bFileMode �򿪷�ʽ
//Return Value:
//	0	��ʧ��
//	>=0	�򿪳ɹ�
DWORD fOpen(_In_ char* szFileName, _In_ BYTE bFileMode);

//Description:
//	�����α�
//Paramter: 
//  _In_ DWORD fileId �ļ���
//  _In_ long _Offset ƫ��
//  _In_ int _Origin = SEEK_SET ��ʼλ��
//Return Value:
//	 ִ�гɹ�
//	-1 ִ��ʧ��
DWORD fSeek(_In_ DWORD fileId, _In_ long _Offset, _In_ int _Origin = SEEK_SET);

//Description:
//	��ȡ�Ѵ򿪵��ļ�
//Paramter: 
//	_In_reads_bytes_(_ElementSize* _ElementCount) void * _Buffer ������
//  _In_ size_t _ElementSize Ԫ�صĴ�С
//  _In_ size_t _ElementCount Ԫ�ص�����
//  _In_ DWORD fileId �ļ���
//Return Value:
//	DWORD ��ȡ�ֽ���
DWORD fRead(_Out_writes_bytes_(_ElementSize* _ElementCount) void* _Buffer,
    _In_ size_t _ElementSize,
    _In_ size_t _ElementCount,
    _In_ DWORD fileId);

//Description:
//	�򿪵�ǰ·���µ�һ���ļ�
//Paramter: 
//	_In_reads_bytes_(_ElementSize* _ElementCount) void * _Buffer Ҫд�������
//  _In_ size_t _ElementSize д��Ԫ�صĴ�С
//  _In_ size_t _ElementCount д��Ԫ�ص�����
//  _In_ DWORD fileId �ļ���
//Return Value:
//	DWORD д���ֽ���
DWORD fWrite(_In_reads_bytes_(_ElementSize* _ElementCount) void const* _Buffer,
    _In_ size_t _ElementSize,
    _In_ size_t _ElementCount,
    _In_ DWORD fileId);

//Description:
//	�رյ�ǰ·���µ�һ���ļ�
//Paramter: 
//	_In_ char* szFileName �ļ���
//Return Value:
//	true ִ�гɹ�
//	false ִ��ʧ��
bool fClose(_In_ DWORD fileId);

//Description:
//	�����ļ�
//Paramter: 
//	_In_ char* szFileName �ļ���
//Return Value:
//	true ִ�гɹ�
//	false ִ��ʧ��
bool fSearch(_In_ DWORD fileId);

//Description:
//	���뵱ǰĿ¼�µ�һ����Ŀ¼
//Paramter: 
//	_In_ char* szFileName Ŀ¼��
//Return Value:
//	true ִ�гɹ�
//	false ִ��ʧ��
bool dIntoSub(_In_ char* szFileName);

//Description:
//	�ڵ�ǰ·���´���һ���ļ���
//Paramter: 
//	_In_ char* szFileName Ҫ�������ļ�����
//Return Value:
//	true ִ�гɹ�
//	false ִ��ʧ��
bool dCreate(_In_ char* szFileName);

//Description:
//	�ڵ�ǰ·���´���һ���ļ���
//Paramter: 
//	_In_ User& user Ҫ�������û�
//Return Value:
//	true ִ�гɹ�
//	false ִ��ʧ��
bool dCreate(_In_ User& user);

//Description:
//	�ڵ�ǰ·����ɾ��һ���ļ����µ��ļ�
//Paramter: 
//	_In_ char* szFileName Ҫɾ�����ļ�����
//	_In_ bool isDeleteDir �Ƿ�ɾ���ļ��нṹ
//	_In_ bool isRecursive �Ƿ�ݹ�ɾ��
//Return Value:
//	true ִ�гɹ�
//	false ִ��ʧ��
bool dDelete(_In_ char* szFileName, _In_ bool isDeleteDir = false, _In_ bool isRecursive = false);

//Description:
//	��ʾ��ǰ·��
//Paramter: 
//	None
//Return Value:
//	None
void showCurrentDirectory();

//Description:
//	��ʾ��ǰĿ¼���������ļ��м����ļ�
//Paramter: 
//	None
//Return Value:
//	None
void showDir();

//Description:
//	�ݹ���ʾ��ǰĿ¼�µ�Ŀ¼��
//Paramter: 
//	None
//Return Value:
//	None
void showDirTree();