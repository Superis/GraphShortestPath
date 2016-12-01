/*
 * components.h
 *
 *  Created on: Dec 1, 2016
 *      Author: alex
 */

#ifndef COMPONENTS_H_
#define COMPONENTS_H_

#include "buffer.h"

#define NUM 20
#define ROWS 50

struct Component {
	int componentID; //current component id
	int nodesSum; //number of nodes in component
	int* includedNodesID; //ids of included nodes
};

struct ComponentCursor {
	Component* component_ptr;
};

class SCC {
	Component* components; // Components index - a vector which storesthe components information
	int componentsCount;
	int componentIDs[NUM]; //inverted index
public:
	SCC* EstimateSCC(IndexNode* );
	int FindNodeSCC_ID(ComponentCursor* );
	bool NextSCC_ID(ComponentCursor* );
	int EstimateShortestPathSCC(Buffer* ,int ,int );
	bool DestroySCC();
};

struct UpdateIndex {
	int index[ROWS][2];
};

class CC {
	uint32_t ccindex[]; //CCIndex
	UpdateIndex* updateIndex;
	uint32_t metricVal;
public:
	CC* EstimateCC(Buffer* );
	bool InsertNewEdge(int ,int );
	int FindNodeCC_ID(CC* components, uint32_t nodeId);
	bool RebuildIndexes();
	bool DestroyCC();
};

#endif
