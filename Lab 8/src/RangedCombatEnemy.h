#pragma once
#ifndef __RANGED_COMBAT_ENEMY__
#define __RANGED_COMBAT_ENEMY__

#include "Agent.h"
#include "DecisionTree.h"

// New in Lab8
#include "Scene.h"

class RangedCombatEnemy final : public virtual Agent
{
public:
	RangedCombatEnemy(Scene* scene);
	~RangedCombatEnemy();

	// Inherited via GameObject
	virtual void draw() override;
	virtual void update() override;
	virtual void clean() override;

	// getters and setters (accessors and mutators)
	float getMaxSpeed() const;
	float getTurnRate() const;
	float getAccelerationRate() const;
	glm::vec2 getDesiredVelocity() const;

	void setMaxSpeed(float speed);
	void setTurnRate(float angle);
	void setAccelerationRate(float rate);
	void setDesiredVelocity(glm::vec2 target_position);

	// public functions
	void Seek();
	void LookWhereYoureGoing(glm::vec2 target_direction);

	// new action functions
	void Flee() override;
	void Patrol() override;
	void MoveToLOS() override;
	void MoveToRange() override;
	void WaitBehindCover() override;
	void MoveToCover() override;
	void Attack() override;

	const DecisionTree* getTree() const;

private:
	// private movement variables
	float m_maxSpeed;
	float m_turnRate;
	float m_accelerationRate;
	// new for Lab 7
	std::vector<glm::vec2> m_patrol;
	int m_waypoint;

	// where we want to go
	glm::vec2 m_desiredVelocity;

	// private functions
	void m_move();

	// Decision Tree
	DecisionTree* m_tree;
	void m_buildTree();

	// New for Lab8
	int m_fireCounter;
	int m_fireCounterMax;
	Scene* m_pScene;
};


#endif /* defined (__RANGED_COMBAT_ENEMY__) */