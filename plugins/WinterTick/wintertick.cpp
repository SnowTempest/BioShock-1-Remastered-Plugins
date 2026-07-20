#include "pch.h"
#include "wintertick.h"
#include "minhookutils.h"
#include "bioglobals.h"
#include "tickconfig.h"

namespace wintertick::wt {

	using namespace wintertick::globals;

	bool g_GEFound = false;
	static int constexpr g_GMTRIndex = 75;
	static int constexpr g_HookRetries = 10;
	static int constexpr g_GEngineInitCheckRetries = 15;

	typedef float(__cdecl* tGetMaxTickRate)();
	tGetMaxTickRate oGetMaxTickRate = nullptr;

	float __cdecl hkGetMaxTickRate() {
		return wintertick::tickconfig::MaxTickRate;
	}

	bool HookVtableIndex() {
		if (!GEngine || !*(void**)GEngine)
			return false;

		void** GEngineVtable = *reinterpret_cast<void***>(*reinterpret_cast<uintptr_t*>(GEngine));

		void* GetMaxTickRateTarget = GEngineVtable[g_GMTRIndex];

		if (!wintertick::minhook::utils::CreateAndEnableHook(GetMaxTickRateTarget, &hkGetMaxTickRate, reinterpret_cast<void**>(&oGetMaxTickRate))) {
			return false;
		}

		return true;
	}

	bool HookGameEngine() {
		if (!HookVtableIndex()) {
			return false;
		}

		g_GEFound = true;
		return true;
	}

	void InitWinterTick() {
		if (!wintertick::minhook::utils::InitMinHook()) {
			return;
		}

		if (!wintertick::globals::InitializeGlobals()) {
			return;
		}

		int engineInitChecks = 0;

		while (!*(void**)GEngine && engineInitChecks < g_GEngineInitCheckRetries) {
			Sleep(500);
			engineInitChecks += 1;
		}

		int hookAttempts = 0;

		while (!g_GEFound && hookAttempts < g_HookRetries) {
			if (HookGameEngine()) {
				break;
			}
			else {
				Sleep(500);
			}

			hookAttempts += 1;
		}
	}
}