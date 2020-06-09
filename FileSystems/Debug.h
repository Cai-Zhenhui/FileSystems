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
//	输出调试调试信息，要要输出的内容长度不能超过2048字节(带换行符)
//Paramter: 
//	TCHAR const* const _Format 格式化字符串
//	... 可变参数
//Return Value:
//	<0  错误
//	>=0	输出的字符串长度
#define Println(_Format,...) DebugPrint(TEXT("调试信息\nFile:%s\nLine:%d Func:%s "##_Format##"\n"),__TFILE__,__LINE__,__TFUNCTION__,__VA_ARGS__)
#else
#define Println(_Format,...)
#endif // _DEBUG


//另一种方案 可以调用两次print
//#define Print2 (DebugPrint(TEXT("调试信息:Line:%d "),__LINE__),DebugPrintln)

int DebugPrint(_In_z_ _Printf_format_string_ TCHAR const* const _Format, ...);

/*
//这是一个遗留问题
#define Print(_Format, ...) test(TEXT("调试信息:Line:%d"),_Format,__LINE__, __VA_ARGS__)
int test(_In_z_ _Printf_format_string_ TCHAR const* const _FormatHead,
	_In_z_ _Printf_format_string_ TCHAR const* const _Format,
	...);*/