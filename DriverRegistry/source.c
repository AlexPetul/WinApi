#include <ntddk.h>
#include "source.h"

LPCSTR GetNotifyClassString(REG_NOTIFY_CLASS NotifyClass)
{
	switch (NotifyClass) {
	case RegNtPreDeleteKey:                 return "RegNtPreDeleteKey";
	case RegNtPreSetValueKey:               return "RegNtPreSetValueKey";
	case RegNtPreDeleteValueKey:            return "RegNtPreDeleteValueKey";
	case RegNtPreSetInformationKey:         return "RegNtPreSetInformationKey";
	case RegNtPreRenameKey:                 return "RegNtPreRenameKey";
	case RegNtPreEnumerateKey:              return "RegNtPreEnumerateKey";
	case RegNtPreEnumerateValueKey:         return "RegNtPreEnumerateValueKey";
	case RegNtPreQueryKey:                  return "RegNtPreQueryKey";
	case RegNtPreQueryValueKey:             return "RegNtPreQueryValueKey";
	case RegNtPreQueryMultipleValueKey:     return "RegNtPreQueryMultipleValueKey";
	case RegNtPreKeyHandleClose:            return "RegNtPreKeyHandleClose";
	case RegNtPreCreateKeyEx:               return "RegNtPreCreateKeyEx";
	case RegNtPreOpenKeyEx:                 return "RegNtPreOpenKeyEx";
	case RegNtPreFlushKey:                  return "RegNtPreFlushKey";
	case RegNtPreLoadKey:                   return "RegNtPreLoadKey";
	case RegNtPreUnLoadKey:                 return "RegNtPreUnLoadKey";
	case RegNtPreQueryKeySecurity:          return "RegNtPreQueryKeySecurity";
	case RegNtPreSetKeySecurity:            return "RegNtPreSetKeySecurity";
	case RegNtPreRestoreKey:                return "RegNtPreRestoreKey";
	case RegNtPreSaveKey:                   return "RegNtPreSaveKey";
	case RegNtPreReplaceKey:                return "RegNtPreReplaceKey";

	case RegNtPostDeleteKey:                return "RegNtPostDeleteKey";
	case RegNtPostSetValueKey:              return "RegNtPostSetValueKey";
	case RegNtPostDeleteValueKey:           return "RegNtPostDeleteValueKey";
	case RegNtPostSetInformationKey:        return "RegNtPostSetInformationKey";
	case RegNtPostRenameKey:                return "RegNtPostRenameKey";
	case RegNtPostEnumerateKey:             return "RegNtPostEnumerateKey";
	case RegNtPostEnumerateValueKey:        return "RegNtPostEnumerateValueKey";
	case RegNtPostQueryKey:                 return "RegNtPostQueryKey";
	case RegNtPostQueryValueKey:            return "RegNtPostQueryValueKey";
	case RegNtPostQueryMultipleValueKey:    return "RegNtPostQueryMultipleValueKey";
	case RegNtPostKeyHandleClose:           return "RegNtPostKeyHandleClose";
	case RegNtPostCreateKeyEx:              return "RegNtPostCreateKeyEx";
	case RegNtPostOpenKeyEx:                return "RegNtPostOpenKeyEx";
	case RegNtPostFlushKey:                 return "RegNtPostFlushKey";
	case RegNtPostLoadKey:                  return "RegNtPostLoadKey";
	case RegNtPostUnLoadKey:                return "RegNtPostUnLoadKey";
	case RegNtPostQueryKeySecurity:         return "RegNtPostQueryKeySecurity";
	case RegNtPostSetKeySecurity:           return "RegNtPostSetKeySecurity";
	case RegNtPostRestoreKey:               return "RegNtPostRestoreKey";
	case RegNtPostSaveKey:                  return "RegNtPostSaveKey";
	case RegNtPostReplaceKey:               return "RegNtPostReplaceKey";

	case RegNtCallbackObjectContextCleanup: return "RegNtCallbackObjectContextCleanup";

	default:
		return "Unsupported REG_NOTIFY_CLASS";
	}
}

BOOLEAN IsLogToFileNeed(REG_NOTIFY_CLASS NotifyClass)
{
	switch (NotifyClass) {
	case RegNtPreSetInformationKey:
	case RegNtPreSetValueKey:
	case RegNtPreDeleteValueKey:
	case RegNtPreDeleteKey:
	case RegNtPreRenameKey:
	case RegNtPreCreateKey:
	case RegNtPreCreateKeyEx:
	case RegNtPreSaveKey:
		return TRUE;
	default:
		return FALSE;
	}
}