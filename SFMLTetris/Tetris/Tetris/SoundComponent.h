#pragma once

#include "Component.h"
#include <string>

#include "SFML/Audio.hpp"

class Tetris;

class SoundComponent : public Component
{
public:
	using Component::Component;

	void QueueSound(std::string szSoundName);
	
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
