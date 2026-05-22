#pragma once

#ifndef GLOBALS_H
#define GLOBALS_H

namespace adamvile::globals {
    typedef void* (__cdecl* tStaticFindObject)(void* Class, void* Package, const wchar_t* Name, bool ExactClass);
    extern tStaticFindObject StaticFindObject;

    extern void* GEngine;

    bool InitializeGlobals();
}

#endif