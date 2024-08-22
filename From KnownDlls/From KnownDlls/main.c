#include "NtdllPatcher.h"

BOOL LoadNtDllFromKnownDlls(OUT PVOID* Ntdll)
{
	NTSTATUS status = 0;
	HANDLE	hSection = NULL;
	BOOL	result = TRUE;

	status = NtOpenSection(&hSection, SECTION_MAP_READ, L"\\KnownDlls\\ntdll.dll");

	if (!NT_SUCCESS(status))
	{
		printf("[-] NtOpenSection Failed: %d\n", GetLastError());
		result = FALSE;
		goto __cleanup;
	}

	*Ntdll = MapViewOfFile(hSection, FILE_MAP_READ, 0, 0, 0);
	if (!*Ntdll)
	{
		printf("[-] MapViewOfFile Failed: %d\n", GetLastError());
		result = FALSE;
		goto __cleanup;
	}

__cleanup:
	CloseHandle(hSection);
	return (result);
}

int main()
{
	HANDLE	hNtdll = NULL;
	PVOID	Ntdll = NULL;

	hNtdll = GetModuleHandleW(L"Ntdll");
	if (IsHooked(hNtdll, "NtOpenProcess") || IsHooked(hNtdll, "NtWriteVirtualMemory") ||
		IsHooked(hNtdll, "NtAllocateVirtualMemory") || IsHooked(hNtdll, "NtCreateThreadEx"))
	{
		puts("[!] One Of The Used API is Hooked");
		puts("[*] Press <ENTER> To Patch Ntdll");
		getchar();

		// Load Ntdll From KnownDlls Directory Object
		if (!LoadNtDllFromKnownDlls(&Ntdll))
		{
			puts("[-] Failed to Load Ntdll From KnownDlls Directory");
			goto __Cleanup;
		}
		// Patch Ntdll Text Section
		if (!NtdllPatcher(hNtdll, Ntdll)) {
			puts("[-] Failed to Patch Ntdll");
			goto __Cleanup;
		}
	}

	puts("[+] You're Good To Go!");
	getchar();

__Cleanup:
	UnmapViewOfFile(Ntdll);
	return (0);
}
