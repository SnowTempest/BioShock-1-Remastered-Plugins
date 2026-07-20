#pragma once

#ifndef MINHOOKUTILS_H
#define MINHOOKUTILS_H

#include "MinHook.h"
#pragma comment(lib, "libMinHook.x86.lib")

namespace wintertick::minhook::utils {
	bool InitMinHook();
	bool CreateAndEnableHook(void* hookTarget, void* detourFunction, void** originalFunction);
}

#endif