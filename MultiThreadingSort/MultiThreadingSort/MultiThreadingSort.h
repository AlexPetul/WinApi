#pragma once
#include <algorithm>
#include <string>
#include <queue>
#include <iostream>
#include <Windows.h>

#define DEFAULT_TASKS_COUNT 5
#define MAX_THREAD_COUNT 5

void FillQueue(std::string * arr, int size);
std::string* SplitText(const std::string& str, std::string* resArr, const std::string& delimiter);
std::string ConvertArrToStr(CHAR* buff, int size);
int ReadFile();

VOID CreateAndStartThreads();
DWORD WINAPI ThreadFunction(LPVOID lpParam);