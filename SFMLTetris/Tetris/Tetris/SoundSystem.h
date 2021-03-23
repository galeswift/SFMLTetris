#include "System.h"

class SoundSystem : public System
{
public:
	virtual void Update(float dt);

private:
	void PlaySounds(SoundComponent* pSoundCmp);
	void LoadSounds(SoundComponent* pSoundCmp);
};
