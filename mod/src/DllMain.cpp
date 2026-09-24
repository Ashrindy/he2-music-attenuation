#include "MusicAttenuationService.h"
#include "OptionsBootstrap.h"

#ifdef PROJECT_TARGET_SDK_rangers
static constexpr size_t GameApplication_ResetAddr = 0x14FEF16E0;
#elif PROJECT_TARGET_SDK_miller
static constexpr size_t GameApplication_ResetAddr = 0x145E64E30;
#endif

HOOK(uint64_t, __fastcall, GameApplication_Reset, GameApplication_ResetAddr, hh::game::GameApplication* self) {
	auto res = originalGameApplication_Reset(self);

	auto* gameManager = hh::game::GameManager::GetInstance();
	auto* s = gameManager->CreateService<MusicAttenuationService>(hh::fnd::MemoryRouter::GetModuleAllocator());
	gameManager->RegisterService(s);

	return res;
}

#ifdef PROJECT_TARGET_SDK_rangers
static constexpr size_t SoundManager_SetMasterVolumeAddr = 0x140BD0C20;
#elif PROJECT_TARGET_SDK_miller
static constexpr size_t SoundManager_SetMasterVolumeAddr = 0x1408983A0;
#endif

HOOK(void, __fastcall, SoundManager_SetMasterVolume, SoundManager_SetMasterVolumeAddr, hh::snd::SoundManager* self, unsigned int category, float volume, float unk0) {
	if (category != MUTE_MUSIC_CATEGORY) {
		originalSoundManager_SetMasterVolume(self, category, volume, unk0);
		return;
	}
	auto* musicAtt = hh::game::GameManager::GetInstance()->GetService<MusicAttenuationService>();
	if (!musicAtt->isPlaying)
		originalSoundManager_SetMasterVolume(self, category, volume, unk0);
}

BOOL WINAPI DllMain(_In_ HINSTANCE hInstance, _In_ DWORD reason, _In_ LPVOID reserved)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		INSTALL_HOOK(GameApplication_Reset);
		INSTALL_HOOK(SoundManager_SetMasterVolume);
		bootstrapOptions();
		break;
	case DLL_PROCESS_DETACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}

	return TRUE;
}
