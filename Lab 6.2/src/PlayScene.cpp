#include "PlayScene.h"
#include "Game.h"
#include "EventManager.h"

// required for IMGUI
#include "imgui.h"
#include "imgui_sdl.h"
#include "Renderer.h"

#include "Util.h"
#include "Config.h"

PlayScene::PlayScene()
{
	PlayScene::start();
}

PlayScene::~PlayScene()
= default;

void PlayScene::draw()
{
	drawDisplayList();
	
	SDL_SetRenderDrawColor(Renderer::Instance().getRenderer(), 255, 255, 255, 255);
}

void PlayScene::update()
{
	updateDisplayList();
	m_checkShipLOS(m_pTarget);
}

void PlayScene::clean()
{
	removeAllChildren();
}

void PlayScene::handleEvents()
{
	EventManager::Instance().update();

	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_ESCAPE))
	{
		TheGame::Instance().quit();
	}

	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_1))
	{
		TheGame::Instance().changeSceneState(START_SCENE);
	}

	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_2))
	{
		TheGame::Instance().changeSceneState(END_SCENE);
	}
}

void PlayScene::start()
{
	// Set GUI Title
	m_guiTitle = "Play Scene";

	m_pTarget = new Target();
	m_pTarget->getTransform()->position = glm::vec2(600.0f, 300.0f);
	addChild(m_pTarget);

	m_pSpaceShip = new SpaceShip();
	m_pSpaceShip->getTransform()->position = glm::vec2(150.f, 300.0f);
	addChild(m_pSpaceShip, 3);

	m_pObstacle1 = new Obstacle();
	m_pObstacle1->getTransform()->position = glm::vec2(380.0f, 80.0f);
	m_pObstacle1->setWidth(50);
	m_pObstacle1->setHeight(50);
	addChild(m_pObstacle1);

	m_pObstacle2 = new Obstacle();
	m_pObstacle2->getTransform()->position = glm::vec2(380.0f, 280.0f);
	m_pObstacle2->setWidth(100);
	addChild(m_pObstacle2);

	m_pObstacle3 = new Obstacle();
	m_pObstacle3->getTransform()->position = glm::vec2(380.0f, 480.0f);
	addChild(m_pObstacle3);
	
	// Setup the Grid
	m_isGridEnabled = false;
	m_storeObstacles();
	m_buildGrid();
	m_toggleGrid(m_isGridEnabled);

	// preload all sounds
	SoundManager::Instance().load("../Assets/audio/yay.ogg", "yay", SOUND_SFX);
	SoundManager::Instance().load("../Assets/audio/thunder.ogg", "thunder", SOUND_SFX);


	ImGuiWindowFrame::Instance().setGUIFunction(std::bind(&PlayScene::GUI_Function, this));
}

void PlayScene::m_buildGrid()
{
	const auto tile_size = Config::TILE_SIZE;
	auto offset = glm::vec2(Config::TILE_SIZE * 0.5f, Config::TILE_SIZE * 0.5f);

	// clearNodes that exist - because when we rebuild/redraw the grid to move or resize an obstacle
	m_clearNodes();
	
	// add path_nodes to the grid
	for (int row = 0; row < Config::ROW_NUM; ++row)
	{
		for (int col = 0; col < Config::COL_NUM; ++col)
		{
			PathNode* path_node = new PathNode();
			path_node->getTransform()->position = glm::vec2((col * tile_size) + offset.x, (row * tile_size) + offset.y);
			bool keepNode = true;
			for (auto obstacle : m_pObstacles)
			{
				if(CollisionManager::AABBCheckWithBuffer(path_node, obstacle, m_obstacleBuffer))
				{
					keepNode = false; // we have collision between node and an obstacle
				}
			}
			if(keepNode)
			{
				addChild(path_node);
				m_pGrid.push_back(path_node);
			}
			else
			{
				delete path_node;
			}
		}
	}

	// if Grid is supposed to be hidden, make it so!
	m_toggleGrid(m_isGridEnabled);
}

void PlayScene::m_toggleGrid(const bool state)
{
	for (auto path_node : m_pGrid)
	{
		path_node->setVisible(state);
	}
}

void PlayScene::m_checkShipLOS(DisplayObject* target_object)
{
	m_pSpaceShip->setHasLOS(false);

	// if ship to target distance is less than or equal to LOS Distance
	auto ShipToTargetDistance = Util::getClosestEdge(m_pSpaceShip->getTransform()->position, target_object);
	if(ShipToTargetDistance <= m_pSpaceShip->getLOSDistance())
	{
		std::vector<DisplayObject*> contactList;
		for (auto object : getDisplayList())
		{
			if (object->getType() == PATH_NODE) { continue; } // ignore path_nodes
			if((object->getType() != m_pSpaceShip->getType()) && (object->getType() != target_object->getType()))
			{
				// check if object is closer to the spaceship than the target
				auto ShipToObjectDistance = Util::getClosestEdge(m_pSpaceShip->getTransform()->position, object);
				if(ShipToObjectDistance <= ShipToTargetDistance)
				{
					contactList.push_back(object);
				}
			}
		}
		auto hasLOS = CollisionManager::LOSCheck(m_pSpaceShip,
			m_pSpaceShip->getTransform()->position + m_pSpaceShip->getCurrentDirection() * m_pSpaceShip->getLOSDistance(),
			contactList, target_object);
		m_pSpaceShip->setHasLOS(hasLOS);
	}
	
}

void PlayScene::m_storeObstacles()
{
	for (auto object : getDisplayList())
	{
		if(object->getType() == OBSTACLE)
		{
			m_pObstacles.push_back(static_cast<Obstacle*>(object));
		}
	}
}

void PlayScene::m_clearNodes()
{
	m_pGrid.clear();
	for (auto object : getDisplayList())
	{
		if(object->getType() == PATH_NODE)
		{
			removeChild(object);
		}
	}
}



void PlayScene::GUI_Function()
{
	// Always open with a NewFrame
	ImGui::NewFrame();

	// See examples by uncommenting the following - also look at imgui_demo.cpp in the IMGUI filter
	//ImGui::ShowDemoWindow();

	auto offset = glm::vec2(Config::TILE_SIZE * 0.5f, Config::TILE_SIZE * 0.5f);

	ImGui::Begin("Lab 6 Debug Properties", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove);

	ImGui::Separator();

	if (ImGui::Checkbox("Toggle Grid", &m_isGridEnabled))
	{
		m_toggleGrid(m_isGridEnabled);
	}	

	ImGui::Separator();

	static int shipPosition[] = { m_pSpaceShip->getTransform()->position.x, m_pSpaceShip->getTransform()->position.y };
	if (ImGui::SliderInt2("Ship Position", shipPosition, 0, 800))
	{
		m_pSpaceShip->getTransform()->position.x = shipPosition[0];
		m_pSpaceShip->getTransform()->position.y = shipPosition[1];
	}

	// allow ship rotation
	static int angle;
	if(ImGui::SliderInt("Ship Direction", &angle, -360, 360))
	{
		m_pSpaceShip->setCurrentHeading(angle);
	}

	ImGui::Separator();

	static int targetPosition[] = { m_pTarget->getTransform()->position.x, m_pTarget->getTransform()->position.y };
	if(ImGui::SliderInt2("Target Position", targetPosition, 0, 800))
	{
		m_pTarget->getTransform()->position.x = targetPosition[0];
		m_pTarget->getTransform()->position.y = targetPosition[1];
	}

	ImGui::Separator();

	for (unsigned i = 0; i < m_pObstacles.size(); ++i)
	{
		int obsPosition[] = { m_pObstacles[i]->getTransform()->position.x, m_pObstacles[i]->getTransform()->position.y };
		std::string label = "Obstacle" + std::to_string(i + 1) + " Position";
		if(ImGui::SliderInt2(label.c_str(), obsPosition, 0, 800))
		{
			m_pObstacles[i]->getTransform()->position.x = obsPosition[0];
			m_pObstacles[i]->getTransform()->position.y = obsPosition[1];
		}
	}

	ImGui::Separator();

	if(ImGui::SliderInt("Obstacle Buffer", &m_obstacleBuffer, 0, 100))
	{
		m_buildGrid();
	}
	
	ImGui::End();
}

int PlayScene::m_obstacleBuffer;


