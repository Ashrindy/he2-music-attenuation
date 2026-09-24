#include "MusicAttenuationService.h"

// All WinRT stuff taken straight from UnleashedRecomp
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Media.Control.h>

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Media::Control;

static GlobalSystemMediaTransportControlsSessionManager g_sessionManager = nullptr;

static GlobalSystemMediaTransportControlsSessionManager GetSessionManager()
{
    if (g_sessionManager)
        return g_sessionManager;
    
    try
    {
        return g_sessionManager = GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
    }
    catch (...)
    {
        return nullptr;
    }
}

static GlobalSystemMediaTransportControlsSession GetCurrentSession()
{
    auto sessionManager = GetSessionManager();

    if (!sessionManager)
        return nullptr;

    try
    {
        return sessionManager.GetCurrentSession();
    }
    catch (...)
    {
        return nullptr;
    }
}

static GlobalSystemMediaTransportControlsSessionPlaybackInfo GetPlaybackInfo()
{
    auto session = GetCurrentSession();

    if (!session)
        return nullptr;

    try
    {
        return session.GetPlaybackInfo();
    }
    catch (...)
    {
        return nullptr;
    }
}

bool IsExternalMediaPlaying()
{
    auto playbackInfo = GetPlaybackInfo();

    if (!playbackInfo)
        return false;

    try
    {
        return playbackInfo.PlaybackStatus() == GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing;
    }
    catch (...)
    {
        return false;
    }
}

const hh::game::GameServiceClass* MusicAttenuationService::GetClass() {
    return &gameServiceClass;
}

MusicAttenuationService::MusicAttenuationService(csl::fnd::IAllocator* allocator) : hh::game::GameService{ allocator } {}

hh::game::GameService* MusicAttenuationService::Create(csl::fnd::IAllocator* allocator) {
    return new (allocator) MusicAttenuationService{ allocator };
}

const hh::game::GameServiceClass MusicAttenuationService::gameServiceClass {
    "MusicAttenuationService",
    &MusicAttenuationService::Create,
    0,
};

int MusicAttenuationThreadImpl(void* userData) {
    init_apartment(winrt::apartment_type::multi_threaded);
    auto* musicAttenuationService = (MusicAttenuationService*)userData;
    while (!musicAttenuationService->stopThread) {
        musicAttenuationService->isPlaying = IsExternalMediaPlaying();
        csl::fnd::ThreadSleep(100);
    }
    g_sessionManager = nullptr;
    return 0;
}

void MusicAttenuationService::OnAddedToGame() {
    gameManager->AddGameUpdateListener(this);
    thread.Create(0, &MusicAttenuationThreadImpl, this, 0x400, 0, "MusicAttenuationThread");
}

void MusicAttenuationService::OnRemovedFromGame() {
    gameManager->RemoveGameUpdateListener(this);
    stopThread = true;
    thread.Exit();
}

void MusicAttenuationService::PreGameUpdateCallback(hh::game::GameManager* gameManager, const hh::fnd::SUpdateInfo& updateInfo) {
    auto* sndPlayer = hh::snd::SoundPlayer::GetInstance();
    if (sndPlayer) {
        float volume = sndPlayer->GetMasterVolume(MUSIC_CATEGORY);
        auto time = 1.0f - expf(2.5f * -updateInfo.deltaTime);
        if (!isActive) {
            // I, uhh.. I don't even know.
            goto label_savedvolume;
        }
        else {
            if (isPlaying)
            {
                volume = std::lerp(volume, 0.0f, time);
            }
            else
            {
label_savedvolume:
                if (auto* saveMgr = gameManager->GetService<app::save::SaveManager>()) {
#ifdef PROJECT_TARGET_SDK_rangers
                    auto optionAcc = saveMgr->GetOptionAccessor();
#elif PROJECT_TARGET_SDK_miller
                    auto optionAcc = saveMgr->GetSystemAccessor();
#endif
                    auto audioAcc = optionAcc.GetOptionAudioAc();
                    volume = std::lerp(volume, audioAcc.GetMusicVolume(), time);
                }
            }
        }
        sndPlayer->SetMasterVolume(MUSIC_CATEGORY, volume);
    }
}

void MusicAttenuationService::SetActive(bool active) {
    isActive = active;
}
