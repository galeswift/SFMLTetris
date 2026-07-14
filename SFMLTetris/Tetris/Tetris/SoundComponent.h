#pragma once

#include "Component.h"
#include <string>

#include "SFML/Audio.hpp"

class SoundComponent : public Component
{
	DECLARE_COMPONENT(COMPONENT_SOUND)

	virtual ~SoundComponent();
	virtual void Reset();

	void QueueSound(std::string szSoundName);
	void UnloadSounds();

	struct LoadedSound
	{
		bool m_queued;
		std::string m_name;
		sf::SoundBuffer *m_pBuffer;
		sf::Sound m_instance;
	};
	std::vector<std::string> m_queuedSounds;
	std::vector<LoadedSound> m_loadedSounds;
};
