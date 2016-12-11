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

enum GRAIL_ANSWER {NO, MAYBE, YES};

struct Label{
	int min_rank;
	int rank;
	label() :min rank(-1),rank(-1){}
}

struct Component {
    Label label;  //Gia ton Grail
	int componentID; //current component id
	int nodesSum; //number of nodes in component
	List<int> includedNodesID; //ids of included nodes
	Component() : componentID(-1), nodesSum(0) {}
};

struct ComponentCursor {
	Component* component_ptr;
};

class SCC {
	Component** components;// Components index - a vector which storesthe components information
	int componentsCount;
    int size;
	//int componentIDs[NUM]; //inverted index den to xreiazomaste exoume metavliti sto index
	int level;
    List<int>* edges; //oi akmes tou hypergraph
public:
	SCC();
	~SCC();
	SCC* EstimateSCC(Buffer* ,Index* ,int);
	int FindNodeSCC_ID(ComponentCursor* );
	bool NextSCC_ID(ComponentCursor* );
	int EstimateShortestPathSCC(Buffer*,Index*,int,int ,int ,int );
	bool DestroySCC();
	void Tarjan(int ,Stack<int>* ,Index* ,Buffer* ,int);
	void Print();
    int Subset(Label a,Label b);
    void BuildHypergraph(Index*,Buffer*);
    void BuildGrailIndex();
    void GrailProgress(int,int**,int*,int*);
    GRAIL_ANSWER IsReachableGrail(Index*,int,int);
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
