#include "CreatingDLL.h"
#include <TlHelp32.h>

using namespace std;

typedef int Func(int, int);
const string strings[COUNT_STRINGS] = {
	"string 1",
	"string 2",
	"string 3",
	"string 4",
	"string 5",
};
const wstring victimProcessName = L"Victimprocess.exe";


int main()
{
	ExecuteStaticImport();
	if (ExecuteDynamicImport() != 0)
	{
		return 1;
	}
	ReplaceString();
	InjectDllInProcess();
	return 0;
}

void ExecuteStaticImport()
{
	cout << "Static Import.\n";
	cout << "Sum(11, 13) = " << Sum(11, 13) << endl;
	cout << "Mul(2, 4) = " << Mul(2, 4) << endl;
}

int ExecuteDynamicImport()
{
	HMODULE lib = LoadLibrary(L"TestDLL");
	cout << "Dynamic Import.\n";
	if (lib != NULL)
	{
		Func* sum, * mul;
		sum = (Func*)GetProcAddress(lib, "Sum");
		mul = (Func*)GetProcAddress(lib, "Mul");
		if (sum != NULL && mul != NULL)
		{
			cout << "Sum(2,121) = " << Sum(2, 121) << endl;
			cout << "Mul(4, 21) = " << Mul(4, 21) << endl;
			FreeLibrary(lib);
			return 0;
		}
		return 1;
	}
	else
	{
		cout << GetLastError();
		return 1;
	}
}

void ReplaceString()
{
	cout << "String before: " << endl;
	for (int index = 0; index < COUNT_STRINGS; index++)
	{
		cout << strings[index].c_str() << endl;
	}
	Replace(strings[3].c_str(), "Hello!!!");
	cout << "After: " << endl;
	for (int index = 0; index < COUNT_STRINGS; index++)
	{
		cout << strings[index].c_str() << endl;
	}
}

int InjectDllInProcess()
{
	_getch();
	HANDLE hRemoteProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetProcessIdByName(victimProcessName));
	if (hRemoteProcess == NULL)
	{
		cout << GetLastError() << endl;
		return 1;
	}
	LPVOID threadFunction = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
	if (threadFunction == NULL)
	{
		cout << GetLastError() << endl;
		return 1;
	}
	string param = "Injection.dll";
	LPVOID argAddress = VirtualAllocEx(hRemoteProcess, NULL, param.length() + 1,
		MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (argAddress == NULL)
	{
		cout << GetLastError() << endl;
		return 1;
	}
	SIZE_T lpl;
	if (WriteProcessMemory(hRemoteProcess, argAddress, param.c_str(), param.length() + 1, &lpl) == 0)
	{
		cout << GetLastError() << endl;
		return 1;
	}
	if (CreateRemoteThread(hRemoteProcess, NULL, 0, (LPTHREAD_START_ROUTINE)threadFunction,
		argAddress, NULL, NULL) == NULL)
	{
		cout << GetLastError() << endl;
		return 1;
	}
	CloseHandle(hRemoteProcess);
	return 0;
}

DWORD GetProcessIdByName(const wstring& processName)
{
	DWORD processId = 0;
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 processEntry;
	ZeroMemory(&processEntry, sizeof(processEntry));
	processEntry.dwSize = sizeof(processEntry);
	bool isFound = false;
	while (Process32Next(hSnapShot, &processEntry) && !isFound)
	{
		if (!processName.compare(processEntry.szExeFile))
		{
			processId = processEntry.th32ProcessID;
			isFound = true;
		}
	}
	return processId;
}