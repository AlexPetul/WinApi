#include <ntifs.h>
#include <ntddk.h>
#include <ntstrsafe.h>
#include <stdlib.h>

#define BUFFER_SIZE 20

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath);
VOID UnloadRoutine(IN PDRIVER_OBJECT DriverObject);
VOID ProcCallback(HANDLE hParentId, HANDLE processId, BOOLEAN bCreate);
NTSTATUS PsLookupProcessByProcessId(HANDLE ProcessId, PEPROCESS *Process);
char* PsGetProcessImageFileName(IN PEPROCESS Process);

#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(PAGE, UnloadRoutine)
 
NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	DriverObject->DriverUnload = UnloadRoutine;
	DbgPrint("Driver loaded!\n");
	PsSetCreateProcessNotifyRoutine(ProcCallback, FALSE);
	return STATUS_SUCCESS;
}
 
VOID UnloadRoutine(IN PDRIVER_OBJECT DriverObject)
{
	PsSetCreateProcessNotifyRoutine(&ProcCallback, TRUE);
	DbgPrint("Driver unloaded!\n");
}

VOID ProcCallback(HANDLE hParentId, HANDLE processId, BOOLEAN bCreate)
{
	UNICODE_STRING sourceProcessName;
	PEPROCESS sourceProcess, pepSourceProcess;
	HANDLE fileHandle, processToTerminate;
	CHAR bufferToWrite[BUFFER_SIZE], bufferToRead[BUFFER_SIZE];
	char* processName;
	size_t cbLen;
	OBJECT_ATTRIBUTES fileAttributes, sourceProjectAttributes;
	IO_STATUS_BLOCK ioStatusBlock;
	CLIENT_ID clientID;
	DWORD pidFromFile;
	NTSTATUS ntstatus;
	LARGE_INTEGER byteOffset;
	if (bCreate == TRUE){
		PsLookupProcessByProcessId(processId, &sourceProcess);
		processName = PsGetProcessImageFileName(sourceProcess);
		ObDereferenceObject(sourceProcess);
		if (strcmp(processName, "test.exe") == 0){
			RtlInitUnicodeString(&sourceProcessName, L"\\DosDevices\\C:\\proc.txt");
			InitializeObjectAttributes(&fileAttributes, &sourceProcessName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
			ntstatus = ZwCreateFile(&fileHandle, GENERIC_WRITE, &fileAttributes, &ioStatusBlock, NULL, FILE_ATTRIBUTE_NORMAL, 0, FILE_OVERWRITE_IF, FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0);
			if (NT_SUCCESS(ntstatus)){
				ntstatus = RtlStringCbPrintfA(bufferToWrite, sizeof(bufferToWrite), "create process");
				if (NT_SUCCESS(ntstatus)){
					ntstatus = RtlStringCbLengthA(bufferToWrite, sizeof(bufferToWrite), &cbLen);
					if (NT_SUCCESS(ntstatus)){
						ntstatus = ZwWriteFile(fileHandle, NULL, NULL, NULL, &ioStatusBlock, bufferToWrite, cbLen, NULL, NULL);
						if (!NT_SUCCESS(ntstatus)){
							DbgPrint("Error writing to file\n");
						}
						DbgPrint("Process %s opened", processName);
						DbgPrint("Process calc.exe started");
						ZwClose(fileHandle);
					}
					else{
						DbgPrint("Error RtlStringCbLengthA\n");
					}
				}
				else{
					DbgPrint("Error RtlStringCbPrintfA\n");
				}
			}
			else{
				DbgPrint("Error opening file \n");
			}
		}
	}
	else{
		PsLookupProcessByProcessId(processId, &pepSourceProcess);
		processName = PsGetProcessImageFileName(pepSourceProcess);
		ObDereferenceObject(pepSourceProcess);
		if (strcmp(processName, "test.exe") == 0){
			DbgPrint("Process %s terminated", processName);
			RtlInitUnicodeString(&sourceProcessName, L"\\DosDevices\\C:\\proc.txt");
			InitializeObjectAttributes(&fileAttributes, &sourceProcessName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
			ntstatus = ZwCreateFile(&fileHandle, GENERIC_READ, &fileAttributes, &ioStatusBlock, NULL, FILE_ATTRIBUTE_NORMAL, 0, FILE_OPEN, FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0);
			if(NT_SUCCESS(ntstatus)) {
				byteOffset.LowPart = byteOffset.HighPart = 0;
				ntstatus = ZwReadFile(fileHandle, NULL, NULL, NULL, &ioStatusBlock, bufferToRead, BUFFER_SIZE, &byteOffset, NULL);
				if(NT_SUCCESS(ntstatus)) {
					bufferToRead[BUFFER_SIZE-1] = '\0';
					pidFromFile = atoi(bufferToRead);
					InitializeObjectAttributes( &sourceProjectAttributes, NULL, OBJ_KERNEL_HANDLE, NULL, NULL );
					clientID.UniqueProcess = (HANDLE)pidFromFile; 
					clientID.UniqueThread = NULL;
					ntstatus = ZwOpenProcess(&processToTerminate, DELETE, &sourceProjectAttributes, &clientID);
					if (NT_SUCCESS(ntstatus)){
						ntstatus = ZwTerminateProcess(processToTerminate, 0);
						if (NT_SUCCESS(ntstatus)){
							ZwClose(processToTerminate);
							DbgPrint("Process calc.exe terminated");
						}
						else{
							DbgPrint("Error terminating target process");
						}
					}
					else{
						DbgPrint("Error opening target process");
					}
				}
				else{
					DbgPrint("Error reading file");
				}
				ZwClose(fileHandle);
			}
			else{
				DbgPrint("Error opening file");
			}
		}
	}
}