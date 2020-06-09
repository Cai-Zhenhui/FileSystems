#pragma once
#include <windows.h>
#include <string>
#include <cstdio>

#ifdef UNICODE
#define tstring wstring
#define tsprintf wsprintf
#define tvsprintf wvsprintf
#define __TFILE__ __FILEW__
#define __TFUNCTION__ __FUNCTIONW__
#else
#define tstring string
#define tsprintf sprintf
#define tvsprintf vsprintf
#define __TFILE__ __FILE__
#define __TFUNCTION__ __FUNCTION__
#endif // !UNICODE

#ifdef _DEBUG
//Description:
//	������Ե�����Ϣ��ҪҪ��������ݳ��Ȳ��ܳ���2048�ֽ�(�����з�)
//Paramter: 
//	TCHAR const* const _Format ��ʽ���ַ���
//	... �ɱ����
//Return Value:
//	<0  ����
//	>=0	������ַ�������
#define Println(_Format,...) DebugPrint(TEXT("������Ϣ\nFile:%s\nLine:%d Func:%s "##_Format##"\n"),__TFILE__,__LINE__,__TFUNCTION__,__VA_ARGS__)
#else
#define Println(_Format,...)
#endif // _DEBUG


//��һ�ַ��� ���Ե�������print
//#define Print2 (DebugPrint(TEXT("������Ϣ:Line:%d "),__LINE__),DebugPrintln)

int DebugPrint(_In_z_ _Printf_format_string_ TCHAR const* const _Format, ...);

/*
//����һ����������
#define Print(_Format, ...) test(TEXT("������Ϣ:Line:%d"),_Format,__LINE__, __VA_ARGS__)
int test(_In_z_ _Printf_format_string_ TCHAR const* const _FormatHead,
	_In_z_ _Printf_format_string_ TCHAR const* const _Format,
	...);*/