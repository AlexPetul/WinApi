#include <ntddk.h>

#define DRIVER_NAME "regdriver"
#define DEVICE_NAME L"\\Device\\regdriver"
#define DEFAULT_ALTITUDE L"100000"
#define TRACKING_PROCESS "regedit.exe"
#define LOG_FILE_DELIMITER " -> "
#define FILE_NEW_LINE "\r\n"

typedef struct _DriverVariables
{
	BOOLEAN isCallbackSet;
	LARGE_INTEGER cookie;
	ANSI_STRING asTrackingProcess;
	ANSI_STRING asDriverName;
	ANSI_STRING asLogFileDelimiter;
	ANSI_STRING asNewLineChar;
	UNICODE_STRING uslogFileName;
} DriverVariables, *PDriverVariables;

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath);
VOID UnloadRoutine(IN PDRIVER_OBJECT DriverObject);
NTSTATUS RegistryOperationsCallback(PVOID CallbackContext, PVOID Argument1, PVOID Argument2);
PDriverVariables GetDriverVariables(PDEVICE_OBJECT pDeviceObject);
NTSTATUS PsLookupProcessByProcessId(HANDLE ProcessId, PEPROCESS *Process);
LPCSTR GetNotifyClassString(REG_NOTIFY_CLASS NotifyClass);
BOOLEAN IsLogToFileNeed(REG_NOTIFY_CLASS NotifyClass);
char* PsGetProcessImageFileName(IN PEPROCESS Process);

#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(PAGE, UnloadRoutine)
#pragma alloc_text(PAGE, RegistryOperationsCallback)
#pragma alloc_text(PAGE, GetDriverVariables)
#pragma alloc_text(PAGE, GetNotifyClassString)
#pragma alloc_text(PAGE, IsLogToFileNeed)
