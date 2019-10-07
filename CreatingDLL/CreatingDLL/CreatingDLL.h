#pragma once
#include <iostream>
#include <conio.h>

#define COUNT_STRINGS 5

#pragma comment(lib, "..\\Debug\\Replace.lib")
#include "..\\ReplaceDLL\\ReplaceDLL.h"

#pragma comment(lib, "..\\Debug\\TestDLL.lib")
#include "..\\TestDLL\\MyDLL.h";

DWORD GetProcessIdByName(const std::wstring& processName);
void ExecuteStaticImport();
int ExecuteDynamicImport();
void ReplaceString();
int InjectDllInProcess();