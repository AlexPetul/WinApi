#pragma once

#include <Windows.h>

extern "C"
{
	__declspec(dllexport) void Replace(LPCSTR target, LPCSTR newStr);
};