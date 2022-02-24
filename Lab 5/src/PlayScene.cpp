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

	// Setup the Grid
	m_buildGrid();
	auto offset = glm::vec2(Config::TILE_SIZE * 0.5f, Config::TILE_SIZE * 0.5f);
	m_currentHeuristic = MANHATTAN;
	
	m_pTarget = new Target();
	m_pTarget->getTransform()->position = m_getTile(15, 11)->getTransform()->position + offset;
	m_pTarget->setGridPosition(15.0f, 11.0f);
	m_getTile(15, 11)->setTileStatus(GOAL);
	addChild(m_pTarget);

	m_pSpaceShip = new SpaceShip();
	m_pSpaceShip->getTransform()->position = m_getTile(1, 3)->getTransform()->position + offset; // position to World
	m_pSpaceShip->setGridPosition(1.0f, 3.0f);
	m_getTile(1, 3)->setTileStatus(START);
	addChild(m_pSpaceShip);

	// preload all sounds
	SoundManager::Instance().load("../Assets/audio/yay.ogg", "yay", SOUND_SFX);
	SoundManager::Instance().load("../Assets/audio/thunder.ogg", "thunder", SOUND_SFX);

	// compute tile costs
	m_computeTileCosts();

	ImGuiWindowFrame::Instance().setGUIFunction(std::bind(&PlayScene::GUI_Function, this));
}

void PlayScene::m_buildGrid()
{
	const auto tile_size = Config::TILE_SIZE;

	// add tiles to the grid
	for (int row = 0; row < Config::ROW_NUM; ++row)
	{
		for (int col = 0; col < Config::COL_NUM; ++col)
		{
			Tile* tile = new Tile(); // create a new empty temp tile
			tile->getTransform()->position = glm::vec2(col * tile_size, row * tile_size); // world position
			tile->setGridPosition(col, row);
			addChild(tile);
			tile->addLabels();
			tile->setEnabled(false);
			m_pGrid.push_back(tile);
		}
	}

	// create neighbour references (connections) for each tile (node) to its neighbours (nodes)
	for (int row = 0; row < Config::ROW_NUM; ++row)
	{
		for (int col = 0; col < Config::COL_NUM; ++col)
		{
			Tile* tile = m_getTile(col, row);

			//TopMost Row?
			if(row == 0)
			{
				tile->setNeighbourTile(TOP_TILE, nullptr);
			}
			else
			{
				tile->setNeighbourTile(TOP_TILE, m_getTile(col, row - 1));
			}

			// RightMost Col?
			if(col == Config::COL_NUM - 1)
			{
				tile->setNeighbourTile(RIGHT_TILE, nullptr);
			}
			else
			{
				tile->setNeighbourTile(RIGHT_TILE, m_getTile(col + 1, row));
			}

			// BottomMost Row?
			if(row == Config::ROW_NUM - 1)
			{
				tile->setNeighbourTile(BOTTOM_TILE, nullptr);
			}
			else
			{
				tile->setNeighbourTile(BOTTOM_TILE, m_getTile(col, row + 1));
			}

			// LeftMost Col?
			if(col == 0)
			{
				tile->setNeighbourTile(LEFT_TILE, nullptr);
			}
			else
			{
				tile->setNeighbourTile(LEFT_TILE, m_getTile(col - 1, row));
			}
		}
	}
	
}

bool PlayScene::m_getGridEnabled() const
{
	return m_isGridEnabled;
}

void PlayScene::m_setGridEnabled(const bool state)
{
	m_isGridEnabled = state;

	for (auto tile : m_pGrid)
	{
		tile->setEnabled(m_isGridEnabled); // enables the Tile GameObject
		tile->setLabelsEnabled(m_isGridEnabled); // enables the corresponding Label Objects
	}
}

void PlayScene::m_computeTileCosts()
{
	float distance = 0.0f;
	float dx = 0.0f;
	float dy = 0.0f;

	// loop through each tile in the grid
	for (auto tile : m_pGrid)
	{
		switch(m_currentHeuristic)
		{
		case MANHATTAN:
			dx = abs(tile->getGridPosition().x - m_pTarget->getGridPosition().x);
			dy = abs(tile->getGridPosition().y - m_pTarget->getGridPosition().y);
			distance = dx + dy;
			break;
		case EUCLIDEAN:
			// compute the euclidean distance (as the crow flies) for each tile to the goal
			distance = Util::distance(tile->getGridPosition(), m_pTarget->getGridPosition());
			break;
		}

		tile->setTileCost(distance);
	}
}

void PlayScene::m_findShortestPath()
{
	// check if pathList is empty
	if(m_pPathList.empty())
	{
		// Step 1. Add Start Position
		Tile* start_tile = m_getTile(m_pSpaceShip->getGridPosition());
		start_tile->setTileStatus(OPEN);
		m_pOpenList.push_back(start_tile);

		bool goal_found = false;

		// Step 2. Loop until the OpenList is empty or the Goal is found
		while(!m_pOpenList.empty() && !goal_found)
		{
			// initialization
			auto min = INFINITY;
			Tile* min_tile;
			int min_tile_index = 0;
			int count = 0;
			std::vector<Tile*> neighbour_list;

			// Step 2.a - Get together the neighbours to check
			// loop through each neighbour in right-winding order (Top - Right - Bottom - Left)
			for (int index = 0; index < NUM_OF_NEIGHBOUR_TILES; ++index)
			{
				const auto neighbour = m_pOpenList[0]->getNeighbourTile(static_cast<NeighbourTile>(index));
				if( neighbour == nullptr || neighbour->getTileStatus() == IMPASSABLE)
				{
					continue; // ignore neighbours that are inappropriate
				}
				neighbour_list.push_back(neighbour);
			}

			// Step 2.b - for every neighbour in the neighbour list
			for (auto neighbour : neighbour_list)
			{
				// Step 2.b1 - check if the neighbour is not the goal
				if(neighbour->getTileStatus() != GOAL)
				{
					// check if neighbour tile cost is less than minimum found so far...
					if(neighbour->getTileCost() < min)
					{
						min = neighbour->getTileCost();
						min_tile = neighbour;
						min_tile_index = count;
					}
					count++;
				}
				else // neighbour is the goal tile
				{
					min_tile = neighbour;
					m_pPathList.push_back(min_tile);
					goal_found = true;
					break;
				}
			}

			// Step 2.c - remove the reference of the current tile in the open list
			m_pPathList.push_back(m_pOpenList[0]); // add the top of the open list to the path_list
			m_pOpenList.pop_back(); // empties the open list

			// Step 2.d - add the min_tile to the openList
			m_pOpenList.push_back(min_tile);
			min_tile->setTileStatus(OPEN);
			neighbour_list.erase(neighbour_list.begin() + min_tile_index); // remove the min_tile from the neighbour list

			// Step 2.e - push all remaining neighbours onto the closed list
			for (auto neighbour : neighbour_list)
			{
				if(neighbour->getTileStatus() == UNVISITED)
				{
					neighbour->setTileStatus(CLOSED);
					m_pClosedList.push_back(neighbour);
				}
			}
		}

		// Alex's hack - to correct the algorithm
		Tile* goal = m_pPathList.at(m_pPathList.size() - 2);
		m_pPathList.erase(m_pPathList.end() - 2);
		m_pPathList.push_back(goal);

		m_displayPathList();
	}
}

void PlayScene::m_displayPathList()
{
	for (auto tile : m_pPathList)
	{
		std::cout << "(" << tile->getGridPosition().x << ", " << tile->getGridPosition().y << ")" << std::endl;
	}
	std::cout << "Path Length: " << m_pPathList.size() << std::endl;
}

void PlayScene::m_resetPathfinding()
{
	
}

void PlayScene::m_resetSimulation()
{
	
}

void PlayScene::m_moveShip()
{
	
}

Tile* PlayScene::m_getTile(const int col, const int row)
{
	return m_pGrid[(row * Config::COL_NUM) + col];
}

Tile* PlayScene::m_getTile(glm::vec2 grid_position)
{
	const auto col = grid_position.x;
	const auto row = grid_position.y;

	return m_pGrid[(row * Config::COL_NUM) + col];
}


void PlayScene::GUI_Function()
{
	auto offset = glm::vec2(Config::TILE_SIZE * 0.5f, Config::TILE_SIZE * 0.5f);
	// Always open with a NewFrame
	ImGui::NewFrame();

	// See examples by uncommenting the following - also look at imgui_demo.cpp in the IMGUI filter
	//ImGui::ShowDemoWindow();
	
	ImGui::Begin("Lab 4 Debug Properties", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove);

	ImGui::Separator();

	static bool toggle_grid = false;
	if (ImGui::Checkbox("Toggle Grid", &toggle_grid))
	{
		m_isGridEnabled = toggle_grid;
		m_setGridEnabled(m_isGridEnabled);
	}
	
	ImGui::Separator();

	// heuristic selection

	static int radio = m_currentHeuristic;
	ImGui::Text("Heuristic Type");
	ImGui::RadioButton("Manhattan", &radio, MANHATTAN);
	ImGui::SameLine();
	ImGui::RadioButton("Euclidean", &radio, EUCLIDEAN);
	
	if(m_currentHeuristic != radio)
	{
		m_currentHeuristic = static_cast<Heuristic>(radio);
		m_computeTileCosts();
	}

	ImGui::Separator();

	if(ImGui::Button("Find Shortest Path"))
	{
		m_findShortestPath();
	}

	ImGui::Separator();

	// target properties

	static int start_position[2] = { m_pSpaceShip->getGridPosition().x, m_pSpaceShip->getGridPosition().y };
	if (ImGui::SliderInt2("Start Position", start_position, 0.0f, Config::COL_NUM - 1))
	{
		// check to ensure that start is not off grid
		if(start_position[1] > Config::ROW_NUM - 1)
		{
			start_position[1] = Config::ROW_NUM - 1;
		}

		m_getTile(m_pSpaceShip->getGridPosition())->setTileStatus(UNVISITED);
		m_pSpaceShip->getTransform()->position = m_getTile(start_position[0], start_position[1])->getTransform()->position + offset;
		m_pSpaceShip->setGridPosition(start_position[0], start_position[1]);
		m_getTile(m_pSpaceShip->getGridPosition())->setTileStatus(START);
	}

	
	static int goal_position[2] = { m_pTarget->getGridPosition().x, m_pTarget->getGridPosition().y};
	if(ImGui::SliderInt2("Goal Position", goal_position, 0.0f, Config::COL_NUM - 1))
	{
		// check to ensure that start is not off grid
		if (goal_position[1] > Config::ROW_NUM - 1)
		{
			goal_position[1] = Config::ROW_NUM - 1;
		}

		m_getTile(m_pTarget->getGridPosition())->setTileStatus(UNVISITED);
		m_pTarget->getTransform()->position = m_getTile(goal_position[0], goal_position[1])->getTransform()->position + offset;
		m_pTarget->setGridPosition(goal_position[0], goal_position[1]);
		m_getTile(m_pTarget->getGridPosition())->setTileStatus(GOAL);
		m_computeTileCosts();
	}

	

	


	
	ImGui::End();
}


