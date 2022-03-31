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
	LOSCondition* getLOSNode() const;
	RadiusCondition* getRadiusNode() const;
	CloseCombatCondition* getCloseCombatNode() const;
	//RangedCombatCondition* getRangedCombatNode() const;
	std::vector<TreeNode*>& getTree();

	void setLOSNode(LOSCondition* node);
	void setRadiusNode(RadiusCondition* node);
	void setCloseCombatNode(CloseCombatCondition* node);
	//void setRangedCombatNode(RangedCombatCondition* node);
	void setAgent(Agent* agent);
	
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
};

#endif /* defined (__DECISION_TREE__) */