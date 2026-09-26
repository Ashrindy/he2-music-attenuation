#include "MusicAttenuationService.h"
#include "OptionsBootstrap.h"

#ifdef PROJECT_TARGET_SDK_rangers
static constexpr size_t GameApplication_ResetAddr = 0x14FEF16E0;
#elif PROJECT_TARGET_SDK_miller
static constexpr size_t GameApplication_ResetAddr = 0x145E64E30;
#elif PROJECT_TARGET_SDK_wars
static constexpr size_t GameApplication_ResetAddr = 0x145400300;
#endif

HOOK(uint64_t, __fastcall, GameApplication_Reset, GameApplication_ResetAddr, hh::game::GameApplication* self) {
	auto res = originalGameApplication_Reset(self);

	auto* gameManager = hh::game::GameManager::GetInstance();
	auto* s = gameManager->CreateService<MusicAttenuationService>(hh::fnd::MemoryRouter::GetModuleAllocator());
	gameManager->RegisterService(s);

	return res;
}

// Sonic Forces (wars) doesn't seem to use the SoundManager, if it even has one. Or most likely, as seen elsewhere, it's been inlined..
#ifndef PROJECT_TARGET_SDK_wars
#ifdef PROJECT_TARGET_SDK_rangers
static constexpr size_t SoundManager_SetMasterVolumeAddr = 0x140BD0C20;
#elif PROJECT_TARGET_SDK_miller
static constexpr size_t SoundManager_SetMasterVolumeAddr = 0x1408983A0;
#endif

HOOK(void, __fastcall, SoundManager_SetMasterVolume, SoundManager_SetMasterVolumeAddr, void* /*hh::snd::SoundManager*/ self, unsigned int category, float volume, float unk0) {
	if (category != MUTE_MUSIC_CATEGORY) {
		originalSoundManager_SetMasterVolume(self, category, volume, unk0);
		return;
	}
	auto* musicAtt = hh::game::GameManager::GetInstance()->GetService<MusicAttenuationService>();
	if (!musicAtt->isPlaying)
		originalSoundManager_SetMasterVolume(self, category, volume, unk0);
}
#else
HOOK(void, __fastcall, SoundPlayer_SetMasterVolume, 0x144ABDD30, void* self, unsigned int category, float volume) {
	if (category == 0x100) {
		originalSoundPlayer_SetMasterVolume(self, 0, volume);
		return;
	}
	if (category != MUTE_MUSIC_CATEGORY)
		originalSoundPlayer_SetMasterVolume(self, category, volume);
}
#endif

BOOL WINAPI DllMain(_In_ HINSTANCE hInstance, _In_ DWORD reason, _In_ LPVOID reserved)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		INSTALL_HOOK(GameApplication_Reset);
#ifndef PROJECT_TARGET_SDK_wars
		INSTALL_HOOK(SoundManager_SetMasterVolume);
#else
		INSTALL_HOOK(SoundPlayer_SetMasterVolume);
#endif
		bootstrapOptions();
		break;
	case DLL_PROCESS_DETACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}

	return TRUE;
}
