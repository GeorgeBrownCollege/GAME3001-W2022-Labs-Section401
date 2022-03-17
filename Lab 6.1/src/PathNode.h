#pragma once
#ifndef __PATH_NODE__
#define __PATH_NODE__

#include "Agent.h"

class PathNode : public Agent
{
public:
	// Constructor
	PathNode();
	
	// Destructor
	~PathNode();
	
	// LifeCycle Functions
	void draw() override;
	void update() override;
	void clean() override;

private:
};

#endif /* defined (__PATH_NODE__) */