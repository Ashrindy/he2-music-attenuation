#pragma once

class MusicAttenuationService : public hh::game::GameService, public hh::game::GameUpdateListener {
public:
	GAMESERVICE_CLASS_DECLARATION(MusicAttenuationService);

	csl::fnd::Thread thread{};
	bool stopThread{ false }; // Super dirty, but it works..?
	bool isPlaying{ false };
	bool isActive{ true };

	virtual void* GetRuntimeTypeInfo() const override { return nullptr; }
	virtual void OnAddedToGame() override;
	virtual void OnRemovedFromGame() override;
	virtual void PreGameUpdateCallback(hh::game::GameManager* gameManager, const hh::fnd::SUpdateInfo& updateInfo) override;
	void SetActive(bool active);
	virtual ~MusicAttenuationService() override {
		thread.Exit();
	}
};
