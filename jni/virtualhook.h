#pragma once

#include <dlfcn.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

namespace VirtualHook
{
	uintptr_t** GetVtable(void*);
	int GetVtableIndex(char*, char*);
	uintptr_t Hook(void**, int, void*);
	uintptr_t Hook(char*, char*, void*);
};
