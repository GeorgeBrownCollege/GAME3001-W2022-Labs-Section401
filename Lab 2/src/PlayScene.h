#pragma once
#ifndef __PLAY_SCENE__
#define __PLAY_SCENE__

#include "Scene.h"

#include "Target.h"
#include "SpaceShip.h"

class PlayScene : public Scene
{
public:
	PlayScene();
	~PlayScene();

	// Scene LifeCycle Functions
	virtual void draw() override;
	virtual void update() override;
	virtual void clean() override;
	virtual void handleEvents() override;
	virtual void start() override;
private:
	// IMGUI Function
	void GUI_Function() const;
	std::string m_guiTitle;
	
	glm::vec2 m_mousePosition;

	// declare GameObjects
	Target* m_pTarget;
	SpaceShip* m_pSpaceShip;
};

#endif /* defined (__PLAY_SCENE__) */