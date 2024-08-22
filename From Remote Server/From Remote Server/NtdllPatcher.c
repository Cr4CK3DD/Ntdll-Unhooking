#include "NtdllPatcher.h"

PIMAGE_SECTION_HEADER GetSection(HMODULE hModule, PCSTR sectionName)
{
	PBYTE               ImageBase = (PBYTE)hModule;
	PIMAGE_DOS_HEADER   DosHdr = (PIMAGE_DOS_HEADER)ImageBase;
	PIMAGE_NT_HEADERS   NtHdr = (PIMAGE_NT_HEADERS)(ImageBase + DosHdr->e_lfanew);

	for (WORD i = 0; i < NtHdr->FileHeader.NumberOfSections; i++)
	{
		PIMAGE_SECTION_HEADER SectionHdr = (PIMAGE_SECTION_HEADER)((DWORD_PTR)IMAGE_FIRST_SECTION(NtHdr) + ((DWORD_PTR)IMAGE_SIZEOF_SECTION_HEADER * i));

		if (!strcmp((PCSTR)SectionHdr->Name, sectionName))
		{
			return (SectionHdr);
		}
	}
	return (NULL);
}

BOOL IsHooked(HANDLE hDll, PCHAR pFunctionName)
{
	PBYTE	pFunctionAddress = NULL;

	pFunctionAddress = (PBYTE)GetProcAddress(hDll, pFunctionName);
	if (pFunctionAddress == NULL) {
		return TRUE;
	}

	if (*pFunctionAddress == 0x4C && *(pFunctionAddress + 1) == 0x8B && *(pFunctionAddress + 2) == 0xD1 &&	// mov r10, rcx
		*(pFunctionAddress + 3) == 0xB8)																	// mov eax, ?? 
	{
		return (FALSE);
	}

	return (TRUE);
}

BOOL NtdllPatcher(HANDLE hDll, PVOID pBuffer)
{
	DWORD	oldProtec = 0;
	DWORD	Result = FALSE;

	PIMAGE_SECTION_HEADER HookedSection = NULL;
	PIMAGE_SECTION_HEADER CleanSection = NULL;

	HookedSection = GetSection(hDll, ".text");
	CleanSection = GetSection((HMODULE)pBuffer, ".text");

	if (!VirtualProtect((BYTE*)hDll + HookedSection->VirtualAddress, CleanSection->SizeOfRawData, PAGE_EXECUTE_READWRITE, &oldProtec)) {
		printf("\t[-] VirtualProtect Failed With: %d\n", GetLastError());
	}

	memcpy((BYTE*)hDll + HookedSection->VirtualAddress, (BYTE*)pBuffer + CleanSection->PointerToRawData, CleanSection->SizeOfRawData);

	if (!VirtualProtect((BYTE*)hDll + HookedSection->VirtualAddress, CleanSection->SizeOfRawData, oldProtec, &oldProtec)) {
		printf("\t[-] VirtualProtect Failed With: %d\n", GetLastError());
	}

	puts("[+] Hooks Removed!");

	Result = TRUE;

	return Result;
}