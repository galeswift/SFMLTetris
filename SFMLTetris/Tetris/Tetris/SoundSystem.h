#pragma once
#include "System.h"

class SoundComponent;

class SoundSystem : public System
{
public:
	void Update(GameManager& manager, float dt) override;

private:
	void PlaySounds(SoundComponent* pSoundCmp);
	void LoadSounds(SoundComponent* pSoundCmp);
};
