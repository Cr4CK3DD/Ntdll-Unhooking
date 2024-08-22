#pragma once
#include "Native.h"
#include <stdio.h>

BOOL NtdllPatcher(HANDLE hDll, PVOID pBuffer);
BOOL IsHooked(HANDLE hDll, PCHAR pFunctionName);
