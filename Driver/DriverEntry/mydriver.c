#include <ntifs.h>
#include <ntddk.h>
#include <ntstrsafe.h>
#include <stdlib.h>

#define BUFFER_SIZE 20

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath);
VOID UnloadRoutine(IN PDRIVER_OBJECT DriverObject);
VOID ProcCallback(HANDLE hParentId, HANDLE processId, BOOLEAN bCreate);
NTSTATUS PsLookupProcessByProcessId(
  HANDLE    ProcessId,
  PEPROCESS *Process
);
char* PsGetProcessImageFileName(IN PEPROCESS Process);


void RunProcess();

#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(PAGE, UnloadRoutine)
#pragma alloc_text(PAGE, RunProcess)
 
NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
  DriverObject->DriverUnload = UnloadRoutine;
  DbgPrint("Hello world!\n");
  PsSetCreateProcessNotifyRoutine(ProcCallback, FALSE);
  return STATUS_SUCCESS;
}
 
VOID UnloadRoutine(IN PDRIVER_OBJECT DriverObject)
{
  DbgPrint("Goodbye!\n");
}

VOID ProcCallback(HANDLE hParentId, HANDLE processId, BOOLEAN bCreate)
{
	if (bCreate == TRUE){
		UNICODE_STRING uniName;
		PEPROCESS process;
		HANDLE handle;
		CHAR buffer[BUFFER_SIZE];
		char* processName;
		size_t cb;
		OBJECT_ATTRIBUTES objAttr;
		IO_STATUS_BLOCK ioStatusBlock;
		NTSTATUS ntstatus;
		PsLookupProcessByProcessId(processId, &process);
		processName = PsGetProcessImageFileName(process);
		ObDereferenceObject(process);
		if (strcmp(processName, "test.exe") == 0){
			RtlInitUnicodeString(&uniName, L"\\DosDevices\\C:\\proc.txt");
			InitializeObjectAttributes(&objAttr, &uniName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
			ntstatus = ZwCreateFile(&handle, GENERIC_WRITE, &objAttr, &ioStatusBlock, NULL, FILE_ATTRIBUTE_NORMAL, 0, FILE_OVERWRITE_IF, FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0);
			if (NT_SUCCESS(ntstatus)){
				ntstatus = RtlStringCbPrintfA(buffer, sizeof(buffer), "create process");
				if (NT_SUCCESS(ntstatus)){
					ntstatus = RtlStringCbLengthA(buffer, sizeof(buffer), &cb);
					if (NT_SUCCESS(ntstatus)){
						ntstatus = ZwWriteFile(handle, NULL, NULL, NULL, &ioStatusBlock, buffer, cb, NULL, NULL);
						if (!NT_SUCCESS(ntstatus)){
							DbgPrint("Error writing to file\n");
						}
						DbgPrint("Process %s opened", processName);
						DbgPrint("Process calc.exe started");
						ZwClose(handle);
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
		OBJECT_ATTRIBUTES objectAttributes;
		CLIENT_ID clientID;
		UNICODE_STRING uniName;
		DWORD pidFromFile;
		HANDLE handle, processToTerminate;
		CHAR buffer[BUFFER_SIZE];
		size_t cb;
		OBJECT_ATTRIBUTES objAttr;
		char* processName;
		IO_STATUS_BLOCK ioStatusBlock;
		NTSTATUS ntstatus;
		LARGE_INTEGER byteOffset;
		PEPROCESS pProcess;
		PsLookupProcessByProcessId(processId, &pProcess);
		processName = PsGetProcessImageFileName(pProcess);
		ObDereferenceObject(pProcess);
		if (strcmp(processName, "test.exe") == 0){
			DbgPrint("Process %s terminated", processName);
			RtlInitUnicodeString(&uniName, L"\\DosDevices\\C:\\proc.txt");
			InitializeObjectAttributes(&objAttr, &uniName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
			ntstatus = ZwCreateFile(&handle, GENERIC_READ, &objAttr, &ioStatusBlock, NULL, FILE_ATTRIBUTE_NORMAL, 0, FILE_OPEN, FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0);
			if(NT_SUCCESS(ntstatus)) {
				byteOffset.LowPart = byteOffset.HighPart = 0;
				ntstatus = ZwReadFile(handle, NULL, NULL, NULL, &ioStatusBlock, buffer, BUFFER_SIZE, &byteOffset, NULL);
				if(NT_SUCCESS(ntstatus)) {
					buffer[BUFFER_SIZE-1] = '\0';
					pidFromFile = atoi(buffer);
					InitializeObjectAttributes( &objectAttributes, NULL, OBJ_KERNEL_HANDLE, NULL, NULL );
					clientID.UniqueProcess = (HANDLE)pidFromFile; 
					clientID.UniqueThread = NULL;
					ZwOpenProcess(&processToTerminate, DELETE, &objectAttributes, &clientID);
					ZwTerminateProcess(processToTerminate, 0);
					ZwClose(processToTerminate);
				}
				ZwClose(handle);
			}
		}
	}
}