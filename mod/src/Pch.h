#pragma once

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#undef CreateService
#undef max
#undef min

#include <detours.h>
#include <d3d11.h>

#include <cstdint>

#ifdef PROJECT_TARGET_SDK_wars
#include <wars-sdk.h>
#define he2sdk warssdk
#define MUSIC_CATEGORY 0x100
#define MUTE_MUSIC_CATEGORY 0
#endif

#ifdef PROJECT_TARGET_SDK_hite
#include <hite-sdk.h>
#define he2sdk hitesdk
#endif

#ifdef PROJECT_TARGET_SDK_rangers
#include <rangers-sdk.h>
#define he2sdk rangerssdk
#define MUSIC_CATEGORY 0
#define MUTE_MUSIC_CATEGORY 0
#endif

#ifdef PROJECT_TARGET_SDK_miller
#include <miller-sdk.h>
#define he2sdk millersdk
#define MUSIC_CATEGORY 0
#define MUTE_MUSIC_CATEGORY 1
#endif

#include <utilities/Helpers.h>

template<typename T>
void WriteProtected(uintptr_t address, T value) {
	DWORD oldProtect;
	VirtualProtect(reinterpret_cast<void*>(address), sizeof(T), PAGE_EXECUTE_READWRITE, &oldProtect);
	*reinterpret_cast<T*>(address) = value;
	VirtualProtect(reinterpret_cast<void*>(address), sizeof(T), oldProtect, &oldProtect);
}
