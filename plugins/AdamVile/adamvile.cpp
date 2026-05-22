#include "pch.h"
#include "globals.h"
#include "adamvile.h"

namespace adamvile::av {

    using namespace adamvile::globals;

    bool g_GlobalsInit = false;

    void EnableConsole() {
        static int ScriptDataOffset = 0x70;
        static int KeyEventScriptMax = 25;
        static int TalkScriptMax = 72;
        static int GoToStateSize = 11;

        static void* KeyEvent = StaticFindObject(nullptr, nullptr, L"Engine.Console.KeyEvent", false);
        static void* Talk = StaticFindObject(nullptr, nullptr, L"Engine.Console.Talk", false);

        if (!KeyEvent || !Talk) {
            return;
        }

        struct UFunctionScript {
            uint8_t* Data;
            int32_t ArrayNum;
            int32_t ArrayMax;
        };

        auto TalkScript = reinterpret_cast<UFunctionScript*>(reinterpret_cast<uintptr_t>(Talk) + ScriptDataOffset);
        auto KeyEventScript = reinterpret_cast<UFunctionScript*>(reinterpret_cast<uintptr_t>(KeyEvent) + ScriptDataOffset);

        if (!TalkScript || !TalkScript->Data || TalkScript->ArrayNum < TalkScriptMax) {
            return;
        }

        if (!KeyEventScript || !KeyEventScript->Data || KeyEventScript->ArrayMax < KeyEventScriptMax) {
            return;
        }

        uint8_t GoToStateBytes[11];
        memcpy(GoToStateBytes, &TalkScript->Data[29], GoToStateSize);

        DWORD oldProtect;
        if (VirtualProtect(KeyEventScript->Data, KeyEventScriptMax, PAGE_EXECUTE_READWRITE, &oldProtect)) {
            memcpy(KeyEventScript->Data, GoToStateBytes, GoToStateSize);
            KeyEventScript->Data[11] = 0x04;
            KeyEventScript->Data[12] = 0x27;
            KeyEventScript->Data[13] = 0x04;
            KeyEventScript->Data[14] = 0x0B;
            
            KeyEventScript->ArrayNum = 15;
            
            VirtualProtect(KeyEventScript->Data, KeyEventScriptMax, oldProtect, &oldProtect);
        } else {
            return;
        }

        Sleep(200);

        if (VirtualProtect(KeyEventScript->Data, KeyEventScriptMax, PAGE_EXECUTE_READWRITE, &oldProtect)) {
            KeyEventScript->Data[0] = 0x04;
            KeyEventScript->Data[1] = 0x28;
            KeyEventScript->Data[2] = 0x04;
            KeyEventScript->Data[3] = 0x0B;
            
            KeyEventScript->ArrayNum = 4;
            
            VirtualProtect(KeyEventScript->Data, KeyEventScriptMax, oldProtect, &oldProtect);
        }

        // Block exit until tilde is released.
        while (GetAsyncKeyState(VK_OEM_3) & 0x8000) {
            Sleep(10);
        }
    }

    void InitAdamVile() {
        while(!g_GlobalsInit) {
            if (adamvile::globals::InitializeGlobals()) {
                g_GlobalsInit = true;
            } else {
                Sleep(500);
            }
        }

        //If GEngine is not initialized then GObjects is not either. StaticFindObject will crash if GObjects isn't initialized yet.
        while (!*(void**)GEngine) {
            Sleep(500);
        }

        while (true) {
            if (GetAsyncKeyState(VK_OEM_3) & 0x8000) {
                EnableConsole();
            }

            //Give the CPU some breathing room.
            Sleep(10);
        }
    }
}