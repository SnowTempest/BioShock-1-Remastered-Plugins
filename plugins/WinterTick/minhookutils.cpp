#include "pch.h"
#include "MinHookUtils.h"

namespace wintertick::minhook::utils {

    bool InitMinHook() {
        if (MH_Initialize() != MH_OK) {
            return false;
        }

        return true;
    }

    bool CreateAndEnableHook(void* hookTarget, void* detourFunction, void** originalFunction) {
        if (MH_CreateHook(hookTarget, detourFunction, originalFunction) != MH_OK) {
            return false;
        }

        if (MH_EnableHook(hookTarget) != MH_OK) {
            return false;
        }

        return true;
    }
}