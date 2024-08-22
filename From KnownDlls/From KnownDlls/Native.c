#include "Native.h"

FORCEINLINE VOID RtlInitUnicodeString(
	_Out_ PUNICODE_STRING DestinationString,
	_In_opt_z_ PCWSTR SourceString
)
{
	if (SourceString)
		DestinationString->MaximumLength = (DestinationString->Length = (USHORT)(wcslen(SourceString) * sizeof(WCHAR))) + sizeof(UNICODE_NULL);
	else
		DestinationString->MaximumLength = DestinationString->Length = 0;

	DestinationString->Buffer = (PWCH)SourceString;
}

NTSTATUS NtOpenSection(OUT PHANDLE SectionHandle, IN ACCESS_MASK DesiredAccess, IN PWCHAR wObjectName) {

	OBJECT_ATTRIBUTES	oa = { 0 };
	UNICODE_STRING		ObjectName = { 0 };
	NTSTATUS			status = 0;
	NTOPENSECTION		fpNtOpenSection = GetProcAddress(GetModuleHandleW(L"Ntdll"), "NtOpenSection");

	RtlInitUnicodeString(&ObjectName, wObjectName);
	InitializeObjectAttributes(&oa, &ObjectName, OBJ_CASE_INSENSITIVE, NULL, NULL);

	status = fpNtOpenSection(SectionHandle, DesiredAccess, &oa);

	return status;
}
