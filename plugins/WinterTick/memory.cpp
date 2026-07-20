#include "pch.h"
#include "memory.h"
#include <optional>
#include <cstring>

namespace wintertick::memory {
    std::pair<uintptr_t, size_t> GetExeRange() {
        static std::optional<std::pair<uintptr_t, size_t>> range = std::nullopt;
        if (range) {
            return *range;
        }

        HMODULE hMod = GetModuleHandleW(nullptr);
        if (!hMod) {
            return { 0, 0 };
        }

        auto* dosHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(hMod);
        auto* ntHeaders = reinterpret_cast<IMAGE_NT_HEADERS*>(reinterpret_cast<uintptr_t>(hMod) + dosHeader->e_lfanew);

        range = { reinterpret_cast<uintptr_t>(hMod), ntHeaders->OptionalHeader.SizeOfImage };

        return *range;
    }

    uintptr_t PatternScan(const char* pattern, const char* mask) {
        auto [base, size] = GetExeRange();

        if (base == 0) {
            return 0;
        }

        const size_t patternSize = strlen(mask);

        if (patternSize == 0 || patternSize > size) {
            return 0;
        }

        const uint8_t* const begin = reinterpret_cast<const uint8_t*>(base);
        const size_t lastStart = size - patternSize;

        for (size_t i = 0; i <= lastStart; ++i) {
            bool found = true;

            for (size_t j = 0; j < patternSize; ++j) {
                if (mask[j] != '?' && begin[i + j] != static_cast<uint8_t>(pattern[j])) {
                    found = false;
                    break;
                }
            }

            if (found) {
                return reinterpret_cast<uintptr_t>(begin + i);
            }
        }

        return 0;
    }
}