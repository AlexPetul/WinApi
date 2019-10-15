#pragma once
#include <algorithm>
#include <string>
#include <queue>
#include <iostream>
#include <Windows.h>
#include <list>

#define DEFAULT_TASKS_COUNT 5
#define MAX_THREAD_COUNT 5

void FillQueue(std::string * arr, int size);
std::string* SplitText(const std::string& str, std::string* resArr, const std::string& delimiter);
std::string ConvertArrToStr(CHAR* buff, int size);
void MergeSortA(int low, int high);
void MergeA(int low, int mid, int high);
char* ReadFileContent();

VOID CreateAndStartThreads();
DWORD WINAPI ThreadFunction(LPVOID lpParam);