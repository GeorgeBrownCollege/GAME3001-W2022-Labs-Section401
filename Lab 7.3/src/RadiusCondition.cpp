#include "RadiusCondition.h"

RadiusCondition::RadiusCondition(const bool within_radius)
{
	m_name = "Radius Condition";
	setIsWithinRadius(within_radius);
}

RadiusCondition::~RadiusCondition()
= default;

void RadiusCondition::setIsWithinRadius(const bool state)
{
	m_isWithinRadius = state;
}

bool RadiusCondition::Condition()
{
	return m_isWithinRadius;
}
