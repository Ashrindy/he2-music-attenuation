#include "OptionsBootstrap.h"
#include "MusicAttenuationService.h"

// The AudioSettingsData struct that's being saved seems to have 2 unused fields for audio devices, we're taking leverage of the second one as the place to save our boolean.
// This is all in hopes (and dreams) that the game 100% doesn't use it.

static const char* optionNames[]{
	"option_disabled",
	"option_enabled"
};

HOOK(bool, __fastcall, sub_14010B760, 0x14010B760, int64_t a1, unsigned int& index, csl::ut::VariableString& name, csl::ut::MoveArray<hh::fnd::Reference<app::ui::UIOptionToggleData>>& toggleDatas, int* value) {
	toggleDatas.clear();
	
	if (index == 5) {
		auto* allocator = hh::fnd::MemoryRouter::GetDebugAllocator();
		name.Set("option_audio_musicatt", 22, allocator);
		auto* audioData = app::SaveData::GetAudioSettingsData();
		*value = audioData->m_outputAdapterIndex;

		for (unsigned int x = 0; x < 2; x++) {
			app::ui::UIIntegerToggleData* integerOption = new (allocator) app::ui::UIIntegerToggleData{ allocator };
			integerOption->valueName = optionNames[x];
			integerOption->value = x;
			toggleDatas.push_back(integerOption);
		}

		return true;
	}
	
	return originalsub_14010B760(a1, index, name, toggleDatas, value);
}

HOOK(void, __fastcall, sub_14010B520, 0x14010B520, int optionIndex, unsigned int& entryIndex, hh::fnd::Reference<app::ui::UIOptionToggleData>& option) {
	if (entryIndex == 5) {
		auto* audioData = app::SaveData::GetAudioSettingsData();
		audioData->m_outputAdapterIndex = option->value;
		if (auto* musicAtt = hh::game::GameManager::GetInstance()->GetService<MusicAttenuationService>())
			musicAtt->SetActive(audioData->m_outputAdapterIndex == 1);
		return;
	}
	originalsub_14010B520(optionIndex, entryIndex, option);
}

HOOK(void, __fastcall, InitializeSoundConfig, 0x140108C80) {
	originalInitializeSoundConfig();
	auto* audioData = app::SaveData::GetAudioSettingsData();
	if (auto* musicAtt = hh::game::GameManager::GetInstance()->GetService<MusicAttenuationService>())
		musicAtt->SetActive(audioData->m_outputAdapterIndex == 1);
}

HOOK(void, __fastcall, sub_140174080, 0x140174080, int64_t self) {
	originalsub_140174080(self);
	hh::game::GameManager* gameManager = *(hh::game::GameManager**)(self + 32);
	hh::fnd::ResourceLoader* resLoader = *(hh::fnd::ResourceLoader**)(((int64_t)gameManager->pApplication) + 0x90);
	resLoader->LoadResource("text/text_musicatt_lang", hh::fnd::Packfile::GetTypeInfo(), 2, true);
}

HOOK(int64_t, __fastcall, sub_14018FB10, 0x14018FB10, int64_t self, int64_t a2, int* a3) {
	auto res = originalsub_14018FB10(self, a2, a3);
	if (*a3 != -3)
		return res;

	hh::game::GameManager* gameManager = *(hh::game::GameManager**)(self + 32);
	hh::fnd::ResourceLoader* resLoader = *(hh::fnd::ResourceLoader**)(((int64_t)gameManager->pApplication) + 0x90);
	resLoader->LoadResource("text/text_musicatt_lang", hh::fnd::Packfile::GetTypeInfo(), 2, true);
	return res;
}

void bootstrapOptions() {
	WriteProtected<char>(0x14010C3E9, 6); // Overwrites the "for loop" that creates the entries, to loop 6 times
	INSTALL_HOOK(sub_14010B760);
	INSTALL_HOOK(sub_14010B520);
	INSTALL_HOOK(InitializeSoundConfig);
	INSTALL_HOOK(sub_140174080);
	INSTALL_HOOK(sub_14018FB10);
}
