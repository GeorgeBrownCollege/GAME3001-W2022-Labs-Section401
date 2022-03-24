#pragma once
#ifndef __DECISION_TREE__
#define __DECISION_TREE__

#include "Agent.h"
#include "CloseCombatCondition.h"
#include "LOSCondition.h"
#include "RadiusCondition.h"
#include "TreeNode.h"
#include "TreeNodeType.h"

class DecisionTree
{
public:
	// Constructor
	DecisionTree();
	DecisionTree(Agent* agent);

	// Destructor
	~DecisionTree();

	// Getters and Setters (accessors and mutators)
	Agent* getAgent() const;
	void setAgent(Agent* agent);

	// readonly getters
	LOSCondition* getLOSNode() const;
	RadiusCondition* getRadiusNode() const;
	CloseCombatCondition* getCloseCombatNode() const;

	// Public Functions
	TreeNode* AddNode(TreeNode* parent, TreeNode* child_node, TreeNodeType type);
	void Display();
	void Update();
	void clean();

	void MakeDecision(); // in-order tree traversal
private:
	// Private Instance Members
	Agent* m_agent;

	LOSCondition* m_LOSNode;
	RadiusCondition* m_RadiusNode;
	CloseCombatCondition* m_CloseCombatNode;

	std::vector<TreeNode*> m_treeNodeList;
	
	// Private methods
	void m_buildTree();
};

#endif /* defined (__DECISION_TREE__) */