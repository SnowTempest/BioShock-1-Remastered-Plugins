#include "pch.h"
#include "memory.h"

namespace adamvile::memory {
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

        size_t patternSize = strlen(mask);
        const uint8_t* memPos = reinterpret_cast<const uint8_t*>(base);
        const uint8_t* memEnd = memPos + size - patternSize;

        uint8_t startingByte = 0;
        size_t startingOffset = 0;
        
        for (size_t i = 0; i < patternSize; ++i) {
            if (mask[i] != '?') {
                uint8_t b = static_cast<uint8_t>(pattern[i]);
                if (b != 0x00 && b != 0xFF && b != 0x48 && b != 0x24) {
                    startingByte = b;
                    startingOffset = i;
                    break;
                }
            }
        }

        if (startingByte == 0 && mask[startingOffset] == '?') {
            for (size_t i = 0; i < patternSize; ++i) {
                if (mask[i] != '?') {
                    startingByte = static_cast<uint8_t>(pattern[i]);
                    startingOffset = i;
                    break;
                }
            }
        }

        while (memPos < memEnd) {
            auto match = std::find(memPos, memEnd + patternSize, startingByte);
            if (match == memEnd + patternSize) {
                return 0; 
            }

            auto actualStart = match - startingOffset;

            if (actualStart < reinterpret_cast<uint8_t*>(base)) {
                memPos = match + 1;
                continue;
            }

            bool found = true;
            for (size_t i = 0; i < patternSize; ++i) {
                if (mask[i] != '?' && actualStart[i] != static_cast<uint8_t>(pattern[i])) {
                    found = false;
                    break;
                }
            }

            if (found) {
                return reinterpret_cast<uintptr_t>(actualStart);
            }

            memPos = match + 1;
        }

        return 0;
    }
}