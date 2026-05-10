#include "pch.h"
#include "util.h"

namespace dxgi::util {

    namespace {

        void toggle_threads(bool enable) {
            HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
            if (snap == INVALID_HANDLE_VALUE) {
                return;
            }

            THREADENTRY32 entry{};
            entry.dwSize = sizeof(entry);

            if (!Thread32First(snap, &entry)) {
                CloseHandle(snap);
                return;
            }

            const DWORD selfProc = GetCurrentProcessId();
            const DWORD selfThread = GetCurrentThreadId();

            do {
                if (entry.th32OwnerProcessID != selfProc || entry.th32ThreadID == selfThread)
                    continue;

                HANDLE hThread = OpenThread(
                    THREAD_SUSPEND_RESUME | THREAD_GET_CONTEXT | THREAD_SET_CONTEXT,
                    FALSE,
                    entry.th32ThreadID
                );

                if (hThread) {
                    if (enable) {
                        ResumeThread(hThread);
                    }
                    else {
                        SuspendThread(hThread);
                    }
                    CloseHandle(hThread);
                }

            } while (Thread32Next(snap, &entry));

            CloseHandle(snap);
        }

    }

    ThreadFreezeGuard::ThreadFreezeGuard() {
        toggle_threads(false);
    }

    ThreadFreezeGuard::~ThreadFreezeGuard() {
        toggle_threads(true);
    }

}