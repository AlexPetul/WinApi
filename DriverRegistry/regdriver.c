#include <ntddk.h>
#include "source.h"
#include <ntstrsafe.h>

NTSTATUS RegistryOperationsCallback(PVOID CallbackContext, PVOID Argument1, PVOID Argument2)
{
	HANDLE hLogFile;
	NTSTATUS ntstatus;
	PEPROCESS pepProcess;
	ANSI_STRING asOperation;
	char *processName;
	UNICODE_STRING fileName;
	OBJECT_ATTRIBUTES fileAttributes;
	IO_STATUS_BLOCK ioStatusBlock;
	LARGE_INTEGER lInt;
	
	REG_NOTIFY_CLASS notifyClass = (REG_NOTIFY_CLASS)(ULONG_PTR)Argument1;
	HANDLE hCurrentProcId = PsGetCurrentProcessId();
	PsLookupProcessByProcessId(hCurrentProcId, &pepProcess);
	processName = PsGetProcessImageFileName(pepProcess);
	lInt.HighPart = -1;
	lInt.LowPart = FILE_WRITE_TO_END_OF_FILE;
	if (strcmp(processName, TRACKING_PROCESS) == 0){
		if (IsLogToFileNeed(notifyClass) == TRUE){
			RtlInitAnsiString(&asOperation, GetNotifyClassString(notifyClass));
			DbgPrint("RegDriver -> %s(%s)", asOperation.Buffer, TRACKING_PROCESS);
			
			RtlInitUnicodeString(&fileName, L"\\DosDevices\\C:\\log.txt");
			InitializeObjectAttributes(&fileAttributes, &fileName, OBJ_OPENIF, NULL, NULL);
			ntstatus = ZwCreateFile(&hLogFile, FILE_APPEND_DATA, &fileAttributes, &ioStatusBlock, NULL, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, FILE_OPEN_IF, FILE_NON_DIRECTORY_FILE, NULL, 0);
			if (NT_SUCCESS(ntstatus)){ 
				ZwWriteFile(hLogFile, NULL, NULL, NULL, &ioStatusBlock, TRACKING_PROCESS, sizeof(TRACKING_PROCESS), &lInt, NULL);
				ZwWriteFile(hLogFile, NULL, NULL, NULL, &ioStatusBlock, LOG_FILE_DELIMITER, sizeof(LOG_FILE_DELIMITER), &lInt, NULL);
				ZwWriteFile(hLogFile, NULL, NULL, NULL, &ioStatusBlock, asOperation.Buffer, asOperation.Length, &lInt, NULL);
				ZwWriteFile(hLogFile, NULL, NULL, NULL, &ioStatusBlock, FILE_NEW_LINE, 2, &lInt, NULL);
			}
			else{
				DbgPrint("RegDriver -> ZwCreateFile error\n");
			}
			ZwClose(hLogFile);
		}
	}
	return STATUS_SUCCESS;
}	

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	UNICODE_STRING altitude, deviceName;
	NTSTATUS ntstatus;
	PDriverVariables driverVariables;
	PDEVICE_OBJECT deviceObject = NULL;
	
	DriverObject->DriverUnload = UnloadRoutine;
	RtlInitUnicodeString(&deviceName, DEVICE_NAME);
	ntstatus = IoCreateDevice(DriverObject, sizeof(driverVariables), &deviceName, FILE_DEVICE_UNKNOWN, 0, FALSE, &deviceObject);
	if (NT_SUCCESS(ntstatus)){
		if (deviceObject != NULL){
			driverVariables = GetDriverVariables(deviceObject);
			if (driverVariables != NULL){
				RtlInitUnicodeString(&altitude, DEFAULT_ALTITUDE);
				ntstatus = CmRegisterCallback(RegistryOperationsCallback, NULL, &(driverVariables->cookie));
				if (NT_SUCCESS(ntstatus)){
					DbgPrint("RegDriver -> Callback set\n");
					driverVariables->isCallbackSet = TRUE;
				}
				else{
					DbgPrint("RegDriver -> Unable to set callback\n");
					return ntstatus;
				}
			}
			else{
				DbgPrint("RegDriver -> Unable to set callback\n");
				return STATUS_UNSUCCESSFUL;
			}
		}
		else{
			return STATUS_UNEXPECTED_IO_ERROR;
		}	
	}
	else{
		DbgPrint("RegDriver -> IoCreateDevice error\n");
		return ntstatus;
	}
	DbgPrint("RegDriver -> Driver is ready!\n");
	return STATUS_SUCCESS;
}
 
VOID UnloadRoutine(IN PDRIVER_OBJECT DriverObject)
{
	PDriverVariables driverVariables = GetDriverVariables(DriverObject->DeviceObject);
	CmUnRegisterCallback(driverVariables->cookie);
	IoDeleteDevice(DriverObject->DeviceObject);
	DbgPrint("Driver unloaded!\n");
}

PDriverVariables GetDriverVariables(PDEVICE_OBJECT pDeviceObject)
{
	if (pDeviceObject == NULL) {
		return NULL;
	}
	if (pDeviceObject->DeviceExtension == NULL) {
		return NULL;
	}
	else {
		return (PDriverVariables)pDeviceObject->DeviceExtension;
	}
}