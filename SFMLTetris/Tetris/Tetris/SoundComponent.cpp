#include "stdafx.h"

#include "SoundComponent.h"

void SoundComponent::QueueSound(std::string szSoundName)
{
	m_queuedSounds.push_back(szSoundName);
}
