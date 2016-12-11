/*
 * components.h
 *
 *  Created on: Dec 1, 2016
 *      Author: alex
 */

#ifndef COMPONENTS_H_
#define COMPONENTS_H_

#include "buffer.h"
#include "template_list.h"
#include "template_stack.h"

// MACRO using typeof/template.
#define MIN(a,b) \
    ({ typeof (a) _a = (a); \
	typeof (b) _b = (b); \
    _a < _b ? _a : _b; })

// valte oti nomizetai stin tixi ta vala gia arxi
#define NUM 20
#define ROWS 50

struct Component {
	int componentID; //current component id
	int nodesSum; //number of nodes in component
	List<int> includedNodesID; //ids of included nodes
	Component() : componentID(-1), nodesSum(0) {}
};

struct ComponentCursor {
	Component* component_ptr;
};

class SCC {
	List<Component*> components; // Components index - a vector which storesthe components information
	int componentsCount;
	int componentIDs[NUM]; //inverted index
	int level;
public:
	SCC();
	~SCC();
	SCC* EstimateSCC(Buffer* ,Index* ,int);
	int FindNodeSCC_ID(ComponentCursor* );
	bool NextSCC_ID(ComponentCursor* );
	int EstimateShortestPathSCC(Buffer* ,int ,int );
	bool DestroySCC();
	void Tarjan(int ,Stack<int>* ,Index* ,Buffer* ,int);

	void Print();
};

struct UpdateIndex {
	int index[ROWS][2];
};

class CC {
	int ccindex[ROWS]; //CCIndex
	UpdateIndex* updateIndex;
	int metricVal;
public:
	CC* EstimateCC(Buffer* );
	bool InsertNewEdge(int ,int );
	int FindNodeCC_ID(CC* components, uint32_t nodeId);
	bool RebuildIndexes();
	bool DestroyCC();
};

#endif
