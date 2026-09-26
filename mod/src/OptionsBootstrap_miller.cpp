#include "OptionsBootstrap.h"
#include "MusicAttenuationService.h"

// The game likes to leave in "reserved" fields in the save data, we can take use of those for our own custom data. Thank you game.

static const char* names[]{
	"OPTION_SOUND_BGM",
	"OPTION_SOUND_SE",
	"OPTION_SOUND_VOICE",
	"OPTION_SOUND_MUSICATT"
};

static unsigned char originalOptionCount = 3;
static unsigned char newOptionCount = originalOptionCount + 1;
static unsigned char originalLastOptionIndex = 2;
static unsigned char newLastOptionIndex = originalLastOptionIndex + 1;

FUNCTION_PTR(char, __fastcall, UIOptionSubConfig_GetValue, 0x140795370, hh::game::GameObject*);

HOOK(bool, __fastcall, UIOptionSoundConfig_IsIndexInRange, 0x1407A18E0, app::ui::UIOptionSoundConfig* self, int optionIndex) {
	return optionIndex <= newLastOptionIndex;
}

HOOK(void, __fastcall, UIOptionSoundConfig_ApplyChanges, 0x14BE36CC0, app::ui::UIOptionSoundConfig* self) {
	originalUIOptionSoundConfig_ApplyChanges(self);
	for (auto& option : self->options) {
		if (option.id == newLastOptionIndex) {
			auto systemAc = app::save::GetSystemAccessor(self->gameManager);
			auto audioAc = systemAc.GetOptionAudioAc();
			audioAc.data->reserved[0] = UIOptionSubConfig_GetValue(option.subConfig) == 0;
			break;
		}
	}
}

HOOK(char, __fastcall, sub_1407A17B0, 0x1407A17B0, int optionIndex) {
	if (optionIndex == newLastOptionIndex) {
		auto systemAc = app::save::GetSystemAccessor(hh::game::GameManager::GetInstance());
		auto audioAc = systemAc.GetOptionAudioAc();
		return audioAc.data->reserved[0] == 1;
	}
	return originalsub_1407A17B0(optionIndex);
}

FUNCTION_PTR(char, __fastcall, GetSaveValue, 0x1407A17B0, int);

HOOK(void, __fastcall, UIOptionSoundConfig_UOC_UnkFunc21, 0x14BE4E7E0, app::ui::UIOptionSoundConfig* self, int optionIndex, int64_t* a3) {
	originalUIOptionSoundConfig_UOC_UnkFunc21(self, optionIndex, a3);
	if (optionIndex == newLastOptionIndex) {
		*a3 = 0;
		*((int64_t*)((int64_t)a3 + 4)) = GetSaveValue(optionIndex) == 0;
	}
}

HOOK(void, __fastcall, UIOptionSoundConfig_OnValueChanged, 0x1407A1620, app::ui::UIOptionSoundConfig* self, int optionIndex, char value, bool unk) {
	originalUIOptionSoundConfig_OnValueChanged(self, optionIndex, value, unk);

	if (optionIndex != newLastOptionIndex) return;

	if (auto* musicAtt = self->gameManager->GetService<MusicAttenuationService>()) {
		musicAtt->SetActive(!value);
	}
}

HOOK(int, __fastcall, UIOptionSoundConfig_GetOptionCount, 0x1401C4610, app::ui::UIOptionSoundConfig* self) {
	return newOptionCount;
}

FUNCTION_PTR(int, __fastcall, UIOptionSoundConfig_GetOptionCountPTR, 0x1401C4610);

HOOK(int, __fastcall, UIOptionSoundConfig_GetOptionID, 0x1407A1760, app::ui::UIOptionSoundConfig* self, int optionIndex) {
	if (optionIndex > newLastOptionIndex)
		return 0;
	return optionIndex;
}

HOOK(const char*, __fastcall, UIOptionSoundConfig_GetOptionName, 0x1407A1780, app::ui::UIOptionSoundConfig* self, int optionIndex) {
	if (optionIndex > newLastOptionIndex)
		return 0;
	return names[optionIndex];
}

HOOK(void, __fastcall, InitializeSoundConfig, 0x14B294590, hh::game::GameService* /*app::snd::SoundDirector*/ self) {
	originalInitializeSoundConfig(self);
	if (auto* musicAtt = self->gameManager->GetService<MusicAttenuationService>()) {
		auto optionAcc = app::save::GetSystemAccessor(self->gameManager);
		auto audioAcc = optionAcc.GetOptionAudioAc();
		musicAtt->SetActive(audioAcc.data->reserved[0] == 1);
	}
}

// Messy solution for loading the pacs, but the game does this too with RichPresence (probably because Steam can be in a different language compared to the game..)
FUNCTION_PTR(const char*, __fastcall, GetLangID, 0x14C3BA7F0, char);

HOOK(int, __fastcall, sub_14011E590, 0x14011E590, int64_t self, int a2) {
	auto res = originalsub_14011E590(self, a2);
	if (a2 == 4) {
		auto* resLoader = *(hh::fnd::ResourceLoader**)(self + 152);
		hh::fnd::InplaceTempUri uri{ "text/text_musicatt" };
		for (auto x = 0; x < 13; x++) {
			hh::fnd::ResourceLoader::Locale locale{ .localeId = 1, .localeName = GetLangID(x) };
			resLoader->LoadResource(uri, hh::fnd::Packfile::GetTypeInfo(), {.unk1 = 0, .unk2 = 1 }, locale);
		}
	}
	return res;
}

void bootstrapOptions() {
	INSTALL_HOOK(sub_14011E590);
	originalOptionCount = UIOptionSoundConfig_GetOptionCountPTR();
	newOptionCount = originalOptionCount + 1;
	originalLastOptionIndex = originalOptionCount - 1;
	newLastOptionIndex = originalLastOptionIndex + 1;
	INSTALL_HOOK(UIOptionSoundConfig_ApplyChanges);
	INSTALL_HOOK(InitializeSoundConfig);
	INSTALL_HOOK(sub_1407A17B0);
	INSTALL_HOOK(UIOptionSoundConfig_UOC_UnkFunc21);
	INSTALL_HOOK(UIOptionSoundConfig_OnValueChanged);
	INSTALL_HOOK(UIOptionSoundConfig_IsIndexInRange);
	INSTALL_HOOK(UIOptionSoundConfig_GetOptionCount);
	INSTALL_HOOK(UIOptionSoundConfig_GetOptionID);
	INSTALL_HOOK(UIOptionSoundConfig_GetOptionName);
}
