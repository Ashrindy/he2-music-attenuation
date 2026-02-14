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
#endif

#ifdef PROJECT_TARGET_SDK_hite
#include <hite-sdk.h>
#define he2sdk hitesdk
#endif

#ifdef PROJECT_TARGET_SDK_rangers
#include <rangers-sdk.h>
#define he2sdk rangerssdk
#endif

#ifdef PROJECT_TARGET_SDK_miller
#include <miller-sdk.h>
#define he2sdk millersdk
#endif

#include <utilities/Helpers.h>
