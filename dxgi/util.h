#pragma once

#ifndef DXGI_UTIL_H
#define DXGI_UTIL_H

namespace dxgi::util {

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