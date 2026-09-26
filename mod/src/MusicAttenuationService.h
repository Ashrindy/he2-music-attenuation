#pragma once

// This has to be cleaned up, this is simply an eye strain to read..
class MusicAttenuationService : 
	public hh::game::GameService
#ifndef PROJECT_TARGET_SDK_wars
	, public hh::game::GameUpdateListener 
#endif
{
public:
	GAMESERVICE_CLASS_DECLARATION(MusicAttenuationService);

	csl::fnd::Thread thread{};
	bool stopThread{ false }; // Super dirty, but it works..?
	bool isPlaying{ false };
	bool isActive{ true };

#ifndef PROJECT_TARGET_SDK_wars
	virtual void* GetRuntimeTypeInfo() const override { return nullptr; }
#else
	virtual void* GetRuntimeTypeInfo() override { return nullptr; }
#endif
	virtual void OnAddedToGame() override;
	virtual void OnRemovedFromGame() override;
#ifndef PROJECT_TARGET_SDK_wars
	virtual void PreGameUpdateCallback(hh::game::GameManager* gameManager, const hh::fnd::SUpdateInfo& updateInfo) override;
#else
	virtual void Update(const hh::fnd::SUpdateInfo& updateInfo) override;
#endif
	void SetActive(bool active);
	virtual ~MusicAttenuationService() override {
		thread.Exit();
	}
};
