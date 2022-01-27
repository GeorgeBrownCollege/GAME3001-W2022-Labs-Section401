#pragma once
#ifndef __SPACE_SHIP__
#define __SPACE_SHIP__

#include "Agent.h"


class SpaceShip final : public Agent
{
public:
	SpaceShip();
	~SpaceShip();

	// Inherited via GameObject
	virtual void draw() override;
	virtual void update() override;
	virtual void clean() override;

	// getters and setters
	float getMaxSpeed() const;
	float getTurnRate() const;
	glm::vec2 getDesiredVelocity() const;
	float getAccelerationRate() const;

	void setMaxSpeed(float speed);
	void setTurnRate(float angle);
	void setDesiredVelocity(glm::vec2 target_position);
	void setAccelerationRate(float rate);

	// public member functions
	void Seek();

private:
	// private movement variables
	float m_maxSpeed;
	float m_turnRate;
	float m_accelerationRate;

	// where we want to go
	glm::vec2 m_desiredVelocity;

	// private move function
	void m_move();
};


#endif /* defined (__SPACE_SHIP__) */
