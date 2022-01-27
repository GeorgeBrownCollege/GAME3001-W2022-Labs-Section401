#include "SpaceShip.h"

#include "Game.h"
#include "TextureManager.h"
#include "Util.h"

// constructor
SpaceShip::SpaceShip()
{
	TextureManager::Instance().load("../Assets/textures/ncl.png", "space_ship");

	const auto size = TextureManager::Instance().getTextureSize("space_ship");
	setWidth(size.x);
	setHeight(size.y);

	// set default position and other properties
	getTransform()->position = glm::vec2(70.0f, 400.0f);
	getRigidBody()->velocity = glm::vec2(0, 0);
	getRigidBody()->acceleration = glm::vec2(0, 0);
	getRigidBody()->isColliding = false;

	// starting properties
	m_maxSpeed = 10.0f;  // a number of pixels moved per frame
	m_turnRate = 5.0f;  // a number of degrees to rotate per time-step
	m_accelerationRate = 2.0f; // a number of pixels to accelerate per frame

	setType(AGENT);
}

SpaceShip::~SpaceShip()
= default;

void SpaceShip::draw()
{
	// alias for x and y
	const auto x = getTransform()->position.x;
	const auto y = getTransform()->position.y;

	// draw the target
	TextureManager::Instance().draw("space_ship", x, y, 0, 255, true);
}

void SpaceShip::update()
{
	m_move();
}

void SpaceShip::clean()
{
}

float SpaceShip::getMaxSpeed() const
{
	return m_maxSpeed;
}

float SpaceShip::getTurnRate() const
{
	return m_turnRate;
}

glm::vec2 SpaceShip::getDesiredVelocity() const
{
	return m_desiredVelocity;
}

float SpaceShip::getAccelerationRate() const
{
	return m_accelerationRate;
}

void SpaceShip::setAccelerationRate(const float rate)
{
	m_accelerationRate = rate;
}

void SpaceShip::setMaxSpeed(const float speed)
{
	m_maxSpeed = speed;
}

void SpaceShip::setTurnRate(const float angle)
{
	m_turnRate = angle;
}

void SpaceShip::setDesiredVelocity(const glm::vec2 target_position)
{
	m_desiredVelocity = Util::normalize(target_position - getTransform()->position) * m_maxSpeed;
	getRigidBody()->velocity = m_desiredVelocity - getRigidBody()->velocity;
	std::cout << "Desired Velocity: (" << m_desiredVelocity.x << ", " << m_desiredVelocity.y << ")" << std::endl;
}

void SpaceShip::Seek()
{
	// compute the target direction and magnitude

	// normalize the direction

	// implement LookWhereYoureGoing

}

void SpaceShip::m_move()
{
	//Seek();

	// kinematic equation for motion --> pf = pi + vi * (timestep) + 0.5ai * (timestep * timestep)

	auto delta_time = TheGame::Instance().getDeltaTime();

	// get the initial position term
	auto initial_position = getTransform()->position;

	// compute the velocity term
	getRigidBody()->velocity += getCurrentDirection() * getMaxSpeed() * (delta_time);

	// compute the acceleration term
	getRigidBody()->acceleration = (getCurrentDirection() * getAccelerationRate()) * 0.5f * (delta_time);

	getTransform()->position = initial_position + getRigidBody()->velocity + getRigidBody()->acceleration;
}


