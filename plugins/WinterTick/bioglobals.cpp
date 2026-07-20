#include "pch.h"
#include "bioglobals.h"
#include "memory.h"

namespace wintertick::globals {

	void* GEngine = nullptr;
	static int GEngineAddrOffset = 5;

	namespace signatures {
		const char* GEngineSig = "\x83\xE1\xFB\x8B\x15\x00\x00\x00\x00\x89\x8A\x80\x00\x00\x00\x83\x3D";
		const char* GEngineMask = "xxxxx????xxxxxxxx";
	}

	bool InitializeGlobals() {
		auto gEngineScan = wintertick::memory::PatternScan(signatures::GEngineSig, signatures::GEngineMask);

		if (!gEngineScan) {
			return false;
		}

		GEngine = wintertick::memory::GetVariable<void*>(gEngineScan, GEngineAddrOffset);

		if (!GEngine) {
			return false;
		}

		return true;
	}
}