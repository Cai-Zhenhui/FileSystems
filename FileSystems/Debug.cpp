#include "stdafx.h"
#include "Debug.h"
#define BUFFER_SIZE 2048
TCHAR _Buffer[BUFFER_SIZE];

int test(TCHAR const* const _FormatHead, TCHAR const* const _Format, ...)
{
	int _Result;
	va_list _ArgList;

	//连接格式化字符串
	std::tstring format = _FormatHead;
	format += _Format;

	const TCHAR* const pformat = format.c_str();
	
	__crt_va_start(_ArgList, pformat);
	//__crt_va_start(_ArgList, _FormatHead);

	_Result = tvsprintf(_Buffer, pformat, _ArgList);
	if (_Result < 0) {
		__crt_va_end(_ArgList);
		//缓冲区溢出
		return _Result;
	}
	//std::tstring str = TEXT("调试信息:");
	std::tstring str(_Buffer);
	str += TEXT('\n');

	OutputDebugString(str.c_str());
	__crt_va_end(_ArgList);
	return _Result;
}

//Description:
//	输出调试调试信息，要要输出的内容长度不能超过2048字节(带换行符)
//Paramter: 
//	TCHAR const* const _Format 格式化字符串
//	... 可变参数
//Return Value:
//	<0  错误
//	>=0	输出的字符串长度
int DebugPrint(_In_z_ _Printf_format_string_ TCHAR const* const _Format, ...) {
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Format);

	_Result = tvsprintf(_Buffer, _Format, _ArgList);
	if (_Result < 0) {
		__crt_va_end(_ArgList);
		//缓冲区溢出
		return _Result;
	}
	_Buffer[BUFFER_SIZE - 1] = 0;

	OutputDebugString(_Buffer);
	__crt_va_end(_ArgList);
	return _Result;
}