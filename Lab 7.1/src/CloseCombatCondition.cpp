#include "CloseCombatCondition.h"

CloseCombatCondition::CloseCombatCondition(const bool within_combat_range)
{
	m_name = "Close Combat Condition";
	setWithinCombatRange(within_combat_range);
}

CloseCombatCondition::~CloseCombatCondition()
= default;

void CloseCombatCondition::setWithinCombatRange(const bool state)
{
	m_within_combat_range = state;
}

bool CloseCombatCondition::Condition()
{
	return m_within_combat_range;
}

