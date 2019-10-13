#include "MultiThreadingSort.h"

using namespace std;


struct Task {
	string* stringsToSort;
	int countStrings;
};

CHAR buff[1198];
HANDLE mutex;

class ConcurrentQueue {
private:
	queue<Task> safeQueue;
public:
	queue<Task> GetQueue() { return safeQueue; }

	void SinglePush(Task task) { safeQueue.push(task); }

	Task SingleFront() { return safeQueue.front(); }

	void SinglePop() { safeQueue.pop(); }
	
}threadSafeQueue;

int main()
{
	cout << "File content before:" << endl;
	if (ReadFile() == 1)
	{
		return 1;
	}
	else
	{
		string plainText = ConvertArrToStr(buff, 1198);
		int arrSize = count(plainText.begin(), plainText.end(), '\n');
		string* resArr = new string[arrSize + 1];
		resArr = SplitText(plainText, resArr, "\r\n");
		FillQueue(resArr, arrSize + 1);
		delete[] resArr;
		CreateAndStartThreads();
		for (int index = 0; index < MAX_THREAD_COUNT; index++) {
			Task task = threadSafeQueue.SingleFront();
			threadSafeQueue.SinglePop();
			for (int ind = 0; ind < task.countStrings; ind++) {
				cout << task.stringsToSort[ind] << endl;
			}
			cout << endl;
		}
	}
	return 0;
}

VOID CreateAndStartThreads()
{
	DWORD threadIds[MAX_THREAD_COUNT];
	HANDLE hThreadArray[MAX_THREAD_COUNT];
	mutex = CreateMutex(NULL, FALSE, NULL);
	for (int index = 0; index < MAX_THREAD_COUNT; index++) {
		hThreadArray[index] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadFunction, 
			(LPVOID)index, 0, &threadIds[index]);
	}

	WaitForMultipleObjects(MAX_THREAD_COUNT, hThreadArray, TRUE, INFINITE);
	for (int index = 0; index < MAX_THREAD_COUNT; index++) {
		CloseHandle(hThreadArray[index]);
	}
}

DWORD WINAPI ThreadFunction(LPVOID lpParam)
{
	DWORD dwWaitResult = WaitForSingleObject(mutex, INFINITE);
	switch (dwWaitResult) {
	case WAIT_OBJECT_0:
		Task currentTask = threadSafeQueue.SingleFront();
		string* stringsToSort = currentTask.stringsToSort;
		int maxLen = currentTask.countStrings;
		for (int index = 0; index < maxLen - 1; index++) {
			for (int ind = 0; ind < maxLen - index - 1; ind++) {
				if (tolower(stringsToSort[ind][0]) > tolower(stringsToSort[ind + 1][0])) {
					string temp = stringsToSort[ind];
					stringsToSort[ind] = stringsToSort[ind + 1];
					stringsToSort[ind + 1] = temp;
				}
			}
		}
		threadSafeQueue.SinglePop();
		threadSafeQueue.SinglePush(Task{ stringsToSort, 3 });
		ReleaseMutex(mutex);
	}
	return 0;
}

void FillQueue(string* arr, int size)
{
	int checkSize = size / DEFAULT_TASKS_COUNT;
	if (checkSize * DEFAULT_TASKS_COUNT != size) {
		int currIndexArr = 0;
		for (int index = 0; index < DEFAULT_TASKS_COUNT; index++) {
			if (index != DEFAULT_TASKS_COUNT - 1) {
				string* newArr = new string[checkSize + 1];
				for (int ind = 0; ind < checkSize; ind++) {
					newArr[ind] = arr[currIndexArr];
					currIndexArr++;
				}
				threadSafeQueue.SinglePush(Task{ newArr, checkSize });
			}
			else {
				int endSize = checkSize + size - 1 - (checkSize * DEFAULT_TASKS_COUNT);
				string* newArr = new string[endSize + 1];
				for (int ind = 0; ind < endSize; ind++) {
					newArr[ind] = arr[currIndexArr];
					currIndexArr++;
				}
				threadSafeQueue.SinglePush(Task{ newArr, endSize });
			}
		}
	}
}

string* SplitText(const string& str, string* resArr, const string& delimiter)
{
	string::size_type lastPos = str.find_first_not_of(delimiter, 0);
	string::size_type pos = str.find_first_of(delimiter, lastPos);
	int arrIndex = 0;
	while (string::npos != pos || string::npos != lastPos) {
		resArr[arrIndex] = str.substr(lastPos, pos - lastPos);
		lastPos = str.find_first_not_of(delimiter, pos);
		pos = str.find_first_of(delimiter, lastPos);
		arrIndex++;
	}
	return resArr;
}

string ConvertArrToStr(CHAR* buff, int size)
{
	string resultStr = "";
	for (int index = 0; index < 1208; index++) {
		resultStr = resultStr + buff[index];
	}
	return resultStr;
}

int ReadFile()
{
	HANDLE hFile = CreateFileA("text.txt", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD fileSize = GetFileSize(hFile, NULL);
	ZeroMemory(buff, sizeof(buff));
	if (hFile == INVALID_HANDLE_VALUE)
	{
		cout << GetLastError() << endl;
		return 1;
	}
	BOOL readResult = ReadFile(hFile, (LPVOID)buff, sizeof(buff), NULL, NULL);
	if (!readResult)
	{
		cout << GetLastError() << endl;
		return 1;
	}
	CloseHandle(hFile);
	return 0;
}