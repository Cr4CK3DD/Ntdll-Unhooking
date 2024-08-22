#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <wininet.h>
#include <stdio.h>

BOOL NtdllPatcher(HANDLE hDll, PVOID pBuffer);
BOOL IsHooked(HANDLE hDll, PCHAR pFunctionName);
