#include "pch.h"
#include "globals.h"
#include "memory.h"

namespace adamvile::globals {

	void* GEngine = nullptr;
	tStaticFindObject StaticFindObject = nullptr;

	namespace signatures {
		const char* GEngineSig = "\x83\xE1\xFB\x8B\x15\x00\x00\x00\x00\x89\x8A\x80\x00\x00\x00\x83\x3D";
		const char* GEngineMask = "xxxxx????xxxxxxxx";
		const char* StaticFindObjectSig = "\x55\x8b\xec\x6a\xff\x68\x00\x00\x00\x00\x64\xa1\x00\x00\x00\x00\x50\x64\x89\x25\x00\x00\x00\x00\x83\xec\x10\x53\x56\x57\x33\xc0";
		const char* StaticFindObjectMask = "xxxxxx????xxxxxxxxxxxxxxxxxxxxxx";
	}

	bool InitializeGlobals() {
		auto gEngineScan = adamvile::memory::PatternScan(signatures::GEngineSig, signatures::GEngineMask);

		if(!gEngineScan) {
			return false;
		}

		GEngine = adamvile::memory::GetVariable<void*>(gEngineScan, 5);

		if(!GEngine) {
			return false;
		}

		auto SFOScan = adamvile::memory::PatternScan(signatures::StaticFindObjectSig, signatures::StaticFindObjectMask);

		if(!SFOScan) {
			return false;
		}

		StaticFindObject = adamvile::memory::GetFunction<tStaticFindObject>(SFOScan);

		if(!StaticFindObject) {
			return false;
		}

		return true;
	}
}