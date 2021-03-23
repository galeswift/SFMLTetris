#include "stdafx.h"

#include "SoundComponent.h"
#include "SoundSystem.h"
#include "Tetris.h"

void SoundSystem::PlaySounds(SoundComponent* pSoundCmp)
{
	for (s32 i = 0; i < pSoundCmp->m_loadedSounds.size(); i++)
	{
		if (pSoundCmp->m_loadedSounds[i].m_queued)
		{
			pSoundCmp->m_loadedSounds[i].m_instance.play();
			pSoundCmp->m_loadedSounds[i].m_queued = false;
		}
	}
}

void SoundSystem::LoadSounds(SoundComponent* pSoundCmp)
{
	for (s32 i = 0; i < pSoundCmp->m_queuedSounds.size(); i++)
	{
		s32 nFoundIdx = -1;
		// Load up all the sounds first
		for (s32 j = 0; j < pSoundCmp->m_loadedSounds.size(); j++)
		{
			if (stricmp(pSoundCmp->m_loadedSounds[j].m_name.c_str(), pSoundCmp->m_queuedSounds[i].c_str()) == 0 )
			{
				nFoundIdx = j;
				break;
			}
		}

		if (nFoundIdx== -1)
		{
			SoundComponent::LoadedSound newSound;
			newSound.m_name = pSoundCmp->m_queuedSounds[i];
			newSound.m_pBuffer = new sf::SoundBuffer();
			newSound.m_pBuffer->loadFromFile(newSound.m_name);
			newSound.m_queued = true;
			newSound.m_instance.setBuffer(*newSound.m_pBuffer);
			pSoundCmp->m_loadedSounds.push_back(newSound);

			nFoundIdx = pSoundCmp->m_loadedSounds.size() - 1;
		}

		pSoundCmp->m_loadedSounds[nFoundIdx].m_queued = true;
	}

	pSoundCmp->m_queuedSounds.clear();
}

void SoundSystem::Update(float dt)
{
	for (s32 i = 0; i < g_clientGame.m_games.size(); i++)
	{
		Tetris* current = g_clientGame.m_games[i]->m_game;
		SoundComponent* soundCmp = current->GetComponent<SoundComponent>();
		if (soundCmp)
		{
			LoadSounds(soundCmp);
			PlaySounds(soundCmp);
		}
	}
}
