#include "MultiThreadingSort.h"

using namespace std;

struct Task {
	string* stringsToSort;
	int countStrings;
};

class ConcurrentQueue {
private:
	queue<Task> safeQueue;
public:
	queue<Task> GetQueue() { return safeQueue; }

	void Enqueue(Task task) { safeQueue.push(task); }

	Task Dequeue() { return safeQueue.front(); }

	void SinglePop() { safeQueue.pop(); }
	
}threadSafeQueue;

HANDLE mutex;
string* resultArr;
DWORD fileSize;

int main()
{
	char* buff = ReadFileContent();
	if (buff == NULL)
	{
		return 1;
	}
	else
	{
		cout << buff << endl << endl;
		string plainText = ConvertArrToStr(buff, fileSize);
		int arrSize = count(plainText.begin(), plainText.end(), '\n');
		string* resArr = new string[arrSize + 1];
		resArr = SplitText(plainText, resArr, "\r\n");
		FillQueue(resArr, arrSize + 1);
		delete[] resArr;
		CreateAndStartThreads();
		resultArr = new string[arrSize + 1];
		int counter = 0;
		for (int index = 0; index < MAX_THREAD_COUNT; index++) {
			Task task = threadSafeQueue.Dequeue();
			string* pointer = threadSafeQueue.Dequeue().stringsToSort;
			threadSafeQueue.SinglePop();
			for (int ind = 0; ind < task.countStrings; ind++) {
				resultArr[counter] = task.stringsToSort[ind];
				counter++;
			}
			delete[] pointer;
		}
		MergeSortA(0, arrSize);
		for (int index = 0; index < arrSize + 1; index++) {
			cout << resultArr[index] << endl;
		}		
		delete[] resultArr;
	}
	return 0;
}

void MergeSortA(int low, int high)
{
	int mid = 0;
	if (low < high) {
		mid = ((low + high) / 2);
		MergeSortA(low, mid);
		MergeSortA(mid + 1, high);
		MergeA(low, mid, high);
	}
}
void MergeA(int low, int mid, int high)
{
	int i = low, j = mid + 1, k = low;
	string *Temp = new string[high + 1];

	while (i <= mid && j <= high) {
		if (resultArr[i] < resultArr[j]) {
			Temp[k].assign(resultArr[i]);
			i++;
		}
		else {
			Temp[k].assign(resultArr[j]);
			j++;
		}
		k++;
	}
	if (i > mid) {
		for (int h = j; h <= high; h++) {
			Temp[k].assign(resultArr[h]);
			k++;
		}
	}
	else {
		for (int h = i; h <= mid; h++) {
			Temp[k].assign(resultArr[h]);
			k++;
		}
	}
	for (int i = low; i <= high; i++) {
		resultArr[i].assign(Temp[i]);
	}
	delete[] Temp;
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
	CloseHandle(mutex);
}

DWORD WINAPI ThreadFunction(LPVOID lpParam)
{
	DWORD dwWaitResult = WaitForSingleObject(mutex, INFINITE);
	switch (dwWaitResult) {
	case WAIT_OBJECT_0:
		Task currentTask = threadSafeQueue.Dequeue();
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
		threadSafeQueue.Enqueue(Task{ stringsToSort, maxLen });
		ReleaseMutex(mutex);
	}
	return 0;
}

void FillQueue(string* arr, int size)
{
	int checkSize = size / DEFAULT_TASKS_COUNT;
	//если поравну разбить не получается
	if (checkSize * DEFAULT_TASKS_COUNT != size) {
		int currIndexArr = 0;
		for (int index = 0; index < DEFAULT_TASKS_COUNT; index++) {
			if (index != DEFAULT_TASKS_COUNT - 1) {
				string* newArr = new string[checkSize + 1];
				for (int ind = 0; ind < checkSize; ind++) {
					newArr[ind] = arr[currIndexArr];
					currIndexArr++;
				}
				threadSafeQueue.Enqueue(Task{ newArr, checkSize });
			}
			else {
				int endSize = checkSize + size - 1 - (checkSize * DEFAULT_TASKS_COUNT);
				string* newArr = new string[endSize + 1];
				for (int ind = 0; ind < endSize; ind++) {
					newArr[ind] = arr[currIndexArr];
					currIndexArr++;
				}
				threadSafeQueue.Enqueue(Task{ newArr, endSize });
			}
		}
	}
	else {
		int currentIndex = 0;
		for (int index = 0; index < DEFAULT_TASKS_COUNT; index++) {
			string* newArr = new string[checkSize + 1];
			for (int ind = 0; ind < checkSize; ind++) {
				newArr[ind] = arr[currentIndex];
			}
			threadSafeQueue.Enqueue(Task{ newArr, checkSize });
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

char* ReadFileContent()
{
	HANDLE hFile = CreateFile(L"text.txt", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	fileSize = GetFileSize(hFile, NULL);
	char* buff = new char[fileSize + 1];
	DWORD bytesRead;
	ZeroMemory(buff, sizeof(buff));
	if (hFile == INVALID_HANDLE_VALUE)
	{
		cout << GetLastError() << endl;
		return NULL;
	}
	BOOL readResult = ReadFile(hFile, (LPVOID)buff, fileSize, &bytesRead, (LPOVERLAPPED)NULL);
	buff[fileSize] = '\0';
	if (!readResult)
	{
		cout << GetLastError() << endl;
		return NULL;
	}
	CloseHandle(hFile);
	return buff;
}