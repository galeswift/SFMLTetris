#include "stdafx.h"

#include "ComponentPool.h"
#include "SoundComponent.h"

DEFINE_COMPONENT(SoundComponent)

SoundComponent::~SoundComponent()
{
	UnloadSounds();
}

void SoundComponent::Reset()
{
	m_queuedSounds.clear();
	UnloadSounds();
}

void SoundComponent::UnloadSounds()
{
	for (s32 i = 0; i < m_loadedSounds.size(); i++)
	{
		m_loadedSounds[i].m_instance.resetBuffer();
		delete m_loadedSounds[i].m_pBuffer;
	}
	m_loadedSounds.clear();
}

void SoundComponent::QueueSound(std::string szSoundName)
{
	m_queuedSounds.push_back(szSoundName);
}
