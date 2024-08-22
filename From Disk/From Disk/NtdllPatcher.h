#pragma once

#include <Windows.h>
#include <stdio.h>

BOOL NtdllPatcher(HANDLE hDll, PVOID pBuffer);
BOOL IsHooked(HANDLE hDll, PCHAR pFunctionName);
