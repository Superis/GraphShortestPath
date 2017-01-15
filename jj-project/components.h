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
    int visited;
    int flag;
	Label() :min_rank(-1),rank(-1),visited(0),flag(0){}
};

struct Component {
    Label label;  //Gia ton Grail
	int componentID; //current component id
	int nodesSum; //number of nodes in component
	List<int>* includedNodesID; //ids of included nodes
	Component() : componentID(-1), nodesSum(0) { includedNodesID=new List<int>;}
};


struct ComponentCursor {
	int cursor;
};

struct TarzanInfoStruct {
	int recursive_level; // numbers the iteration this node was discovered.Mporw na xrisimopoihsw kai src_level kai na to thetw pali -1 sto telos
	int index; // numbers the nodes consecutively in the order in which they are discovered
	int lowlink; // smallest index of any node known to be reachable this->Node,this included
	int parentNode; // node_id which this->node has been called from.
	bool visited;
	TarzanInfoStruct() : recursive_level(0), index(-1),
						 lowlink(-1), parentNode(-1),visited(false) {}
};

class SCC {
	Component** components;// Components index - a vector which storesthe components information
	int componentsCount;
    int size;
	//int componentIDs[NUM]; //inverted index den to xreiazomaste exoume metavliti sto index
	int level;
    List<int>** edges; //oi akmes tou hypergraph
public:
	SCC(int);
	~SCC();
    List<int>** GetStrongEdges(){ return edges;};
    Component** GetComponent(){ return components;};
    int GetCompCount(){ return componentsCount;};
    void AddComponentToArray(Component*);
    void EstimateSCC(Buffer* ,Index* ,int);
	int FindNodeSCC_ID(int,Index*);
	bool NextSCC_ID(ComponentCursor* );
	int EstimateShortestPathSCC(Buffer*,Index*,int,int ,int ,int );
	bool DestroySCC();
    void Tarjan(int ,Stack<int>* ,Index* ,Buffer* ,int, TarzanInfoStruct[], int*);
	void Print();
    int Subset(Label a,Label b);
    void BuildHypergraph(Index*,Buffer*);
    void BuildGrailIndex();
    int GetUnvisitedEdge(int);
    void GrailProgress(int,int*);
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
