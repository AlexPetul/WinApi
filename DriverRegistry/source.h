#include <ntddk.h>

#define LOG_FILE_PATH L"\\??\\C:\\regdriver.log"
#define DRIVER_NAME "regdriver"
#define TRACKING_PROCESS "regedit.exe"
#define LOG_FILE_DELIMITER ":"

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
NTSTATUS RegistryOperationsCallback(PVOID CallbackContext, PVOID Argument1, PVOID Argument2);
VOID UnloadRoutine(IN PDRIVER_OBJECT DriverObject);
void dvInitialize(PDEVICE_OBJECT pDeviceObject);
PDriverVariables GetDriverVariables(PDEVICE_OBJECT pDeviceObject);
typedef PCHAR TPsGetProcessImageFileName(PEPROCESS Process);
NTSTATUS PsLookupProcessByProcessId(HANDLE ProcessId, PEPROCESS *Process);
LPCSTR GetNotifyClassString(REG_NOTIFY_CLASS NotifyClass);
BOOLEAN IsLogToFileNeed(REG_NOTIFY_CLASS NotifyClass);
NTSTATUS PsLookupProcessByProcessId(HANDLE ProcessId, PEPROCESS *Process);
char* PsGetProcessImageFileName(IN PEPROCESS Process);