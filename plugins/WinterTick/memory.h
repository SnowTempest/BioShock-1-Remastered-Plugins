#pragma once

#ifndef MEMORY_H
#define MEMORY_H

#include <utility>

namespace wintertick::memory {
    std::pair<uintptr_t, size_t> GetExeRange();
    uintptr_t PatternScan(const char* pattern, const char* mask);

    template <typename T>
    T* GetVariable(uintptr_t address, uint32_t offsetToPtr) {
        if (!address) {
            return nullptr;
        }

        uintptr_t absolutePointer = *reinterpret_cast<uintptr_t*>(address + offsetToPtr);
        return reinterpret_cast<T*>(absolutePointer);
    }
}

#endif