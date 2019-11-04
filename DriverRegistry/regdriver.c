#include <ntddk.h>
#include "source.h"
#include <ntstrsafe.h>

#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(PAGE, UnloadRoutine)

TPsGetProcessImageFileName *gpPsGetProcessImageFileName;
PDEVICE_OBJECT gpDeviceObject;

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
	CHAR bufferToWrite[100];
	
	REG_NOTIFY_CLASS notifyClass = (REG_NOTIFY_CLASS)(ULONG_PTR)Argument1;
	HANDLE hCurrentProcId = PsGetCurrentProcessId();
	PsLookupProcessByProcessId(hCurrentProcId, &pepProcess);
	processName = PsGetProcessImageFileName(pepProcess);
	if (strcmp(processName, "regedit.exe") == 0){
		if (IsLogToFileNeed(notifyClass) == TRUE){
			RtlInitAnsiString(&asOperation, GetNotifyClassString(notifyClass));
			DbgPrint("%s", asOperation.Buffer);
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
	
	UNICODE_STRING usPsGetProcessImageFileName = RTL_CONSTANT_STRING(L"PsGetProcessImageFileName");
	gpPsGetProcessImageFileName = (TPsGetProcessImageFileName *)MmGetSystemRoutineAddress(&usPsGetProcessImageFileName);
	if (!gpPsGetProcessImageFileName)
	{
		DbgPrint("RegDriver -> PSGetProcessImageFileName not found\n");
		return STATUS_UNSUCCESSFUL;
	}
	
	DriverObject->DriverUnload = UnloadRoutine;
	RtlInitUnicodeString(&deviceName, L"\\Device\\regdriver");
	ntstatus = IoCreateDevice(DriverObject, sizeof(driverVariables), &deviceName, FILE_DEVICE_UNKNOWN, 0, FALSE, &deviceObject);
	if (!NT_SUCCESS(ntstatus)){
		return ntstatus;
	}
	if (deviceObject == NULL){
		return STATUS_UNEXPECTED_IO_ERROR;
	}
	else{
		DbgPrint("RegDriver -> Device successfully created\n");
	}
	dvInitialize(deviceObject);
	driverVariables = GetDriverVariables(deviceObject);
	if (driverVariables == NULL){
		DbgPrint("RegDriver -> cant initialize driver variables\n");
	}
	else{
		DbgPrint("RegDriver -> driver variables initialized!\n");
	}
	RtlInitUnicodeString(&altitude, L"100000");
	ntstatus = CmRegisterCallback(RegistryOperationsCallback, NULL, &(driverVariables->cookie));
	if (NT_SUCCESS(ntstatus)){
		DbgPrint("RegDriver -> Callback set\n");
		driverVariables->isCallbackSet = TRUE;
	}
	else{
		DbgPrint("RegDriver -> Unable to set callback\n");
	}
	DbgPrint("RegDriver -> Driver is ready!\n");
	return STATUS_SUCCESS;
}
 
VOID UnloadRoutine(IN PDRIVER_OBJECT DriverObject)
{
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

void dvInitialize(PDEVICE_OBJECT pDeviceObject)
{
	PDriverVariables pDriverVariables = GetDriverVariables(pDeviceObject);
	if (pDriverVariables == NULL) {
		return;
	}
	RtlInitUnicodeString(&(pDriverVariables->uslogFileName), LOG_FILE_PATH);
	pDriverVariables->isCallbackSet = FALSE;
	RtlInitAnsiString(&(pDriverVariables->asTrackingProcess), TRACKING_PROCESS);
	RtlInitAnsiString(&(pDriverVariables->asDriverName), DRIVER_NAME);
	RtlInitAnsiString(&(pDriverVariables->asLogFileDelimiter), LOG_FILE_DELIMITER);
	RtlInitAnsiString(&(pDriverVariables->asNewLineChar), "\n");
}