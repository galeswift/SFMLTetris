#include "stdafx.h"
#include "AIControllerComponent.h"
#include "AIEvaluatorComponent.h"
#include "AIEvaluatorUtil.h"
#include "AIHeuristics.h"
#include "AIMoveSystem.h"
#include "ClientGame.h"
#include "Tetris.h"

void AIMoveSystem::Update(GameManager& manager, float dt)
{
	for (ComponentIterator itr = manager.GetComponents(COMPONENT_AI_CONTROLLER); itr.Get() != nullptr; itr++)
	{
		AIControllerComponent* controlComp = itr.Get<AIControllerComponent>();

		AIEvaluatorComponent* evalComp = controlComp->m_owner->GetComponent<AIEvaluatorComponent>();
		if (evalComp &&
			evalComp->CanFindMove() &&
			controlComp->NeedsNewMove())
		{
			AIEvaluatorUtil::FindBestMove(evalComp);
			controlComp->SetCurrentMove(evalComp->GetBestMove());
		}
	}
}

void AIMoveSystem::Draw(GameManager& manager, sf::RenderWindow * window, float dt)
{
	// Debug
	for (ComponentIterator itr = manager.GetComponents(COMPONENT_AI_EVALUATOR); itr.Get() != nullptr; itr++)
	{
		AIEvaluatorComponent* evalComp = itr.Get<AIEvaluatorComponent>();
		sf::Text text;
		text.setFont(evalComp->m_owner->m_debugFont);
		text.setCharacterSize(16);
		text.setColor(sf::Color::White);

		for (int j = 0; j < evalComp->m_debugHeuristics.size(); j++)
		{
			std::string textStr = evalComp->m_debugHeuristics[j].m_description;
			textStr += " " + std::to_string(evalComp->m_debugHeuristics[j].m_lastScore);
			text.setPosition(sf::Vector2f(50, (float)250 + j * 20));
			text.setString(textStr.c_str());
			window->draw(text);
		}
	}
}
