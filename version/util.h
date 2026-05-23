#pragma once

#ifndef VERSION_UTIL_H
#define VERSION_UTIL_H

namespace version::util {

    class ThreadFreezeGuard {
    public:
        ThreadFreezeGuard();
        ~ThreadFreezeGuard();

        ThreadFreezeGuard(const ThreadFreezeGuard&) = delete;
        ThreadFreezeGuard(ThreadFreezeGuard&&) = delete;
        ThreadFreezeGuard& operator=(const ThreadFreezeGuard&) = delete;
        ThreadFreezeGuard& operator=(ThreadFreezeGuard&&) = delete;
    };

}

#endif