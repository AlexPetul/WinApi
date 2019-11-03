#include <stdio.h>
#include <Windows.h>
#include <unistd.h>
#include <stdlib.h>
 
int main(int argc,char* argv[])
{
	HANDLE hFile;
	DWORD fileSize, bytesRead, dwWritten;
	OVERLAPPED osWrite = {0};
	char* buff;
	while (1){
		hFile = CreateFile("C:\\proc.txt", GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		fileSize = GetFileSize(hFile, NULL);
		buff = (char*)malloc(fileSize + 1);
		ZeroMemory(buff, sizeof(buff));
		ReadFile(hFile, (LPVOID)buff, fileSize, &bytesRead, (LPOVERLAPPED)NULL);
		buff[fileSize] = '\0';
		if (strcmp(buff, "create process") == 0){ //сигнал для создания процесса
			TCHAR czCommandLine[] = "calc";
			STARTUPINFO siCalc = {sizeof(siCalc)};
			PROCESS_INFORMATION piCalc; 
			if (CreateProcess(NULL, czCommandLine, NULL, NULL, FALSE, 0, NULL, NULL, &siCalc, &piCalc)){
				CloseHandle(hFile);
				HANDLE outFile = CreateFile("C:\\proc.txt", GENERIC_WRITE, FILE_SHARE_WRITE, NULL, TRUNCATE_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
				DWORD processPid = piCalc.dwProcessId;
				DWORD tempPid = processPid;
				int counter = 0;
				while (tempPid != 0){
					tempPid /= 10;
					++counter;
				}
				char buffToWrite[counter + 1];
				itoa(processPid, buffToWrite, 10);
				buffToWrite[counter] = '\0';
				WriteFile(outFile, (LPCVOID)&buffToWrite, sizeof(buffToWrite), &dwWritten, NULL);
				CloseHandle(outFile);
				sleep(10);
				return 0;
			}
		}
		realloc(buff, sizeof(buff));
		sleep(4);
		CloseHandle(hFile);
	}
    return 0;
}