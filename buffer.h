#ifndef BUFFER_H_
#define BUFFER_H_

#include <cstdlib>
#include <stdint.h>
#include "template_queue.h"

#define N 75


struct IndexNode {
	int in; // offset of incoming node
	int inlast; // last offset of incoming neighbors.*Improved speed*
	int inNeighbors; // sum of incoming Neighbors.*Improved speed*
	int out;
	int outlast;
	int outNeighbors;
	int* src_level; // numbers the iteration in which this Node was discovered from source Node
	int* dest_level;
	int* src_visited;
	int* dest_visited;
	// Part 2 Additions :
	int componentID;

	IndexNode() :
			in(-1), inlast(-1), inNeighbors(0), out(-1), outlast(-1),
			outNeighbors(0), src_level(NULL), dest_level(NULL),src_visited(NULL),dest_visited(NULL), componentID(-1) {}
};

class Buffer;
// Avoiding Circular Dependencies of header files
class SCC;
class CC;

class Index {
	IndexNode *indexArray; // Dynamic array of graph indices
	int indSize;
public:
	Index(int);
	~Index();

	IndexNode* GetIndexNode();
	int GetSize() { return indSize; };
	int GetNeighbor(int ,Buffer* ,int);
	void InitializeVisited(int);
	int NeighboursNum(int, char, Buffer*);
	void Insert(int, int, Buffer *);
	void Reallocate(int); // increase capacity of Index data type till == "int" | Realloc
	void CheckCap(int, int);

	void Print();
};

class Node {
	int neighbor[N], //the ids of the neighbor nodes
	endPos; // pointing to first empty cell of array.When full endpos == maxcapcity
	int maxCapacity; // maxCapacity of arrays|Needed for reallocation.
	int edgeProperty[N]; //property for each edge
	int nextNode;
public:
	Node();
	~Node();

	int GetNextNode();
	int GetNeighbor(int);
	int GetCapacity() { return maxCapacity; };
	int GetEndPos() { return endPos; };

	void SetNextNode(int);
	int SearchDiffComponent(int, SCC*,Index*);
	int AddNeighbor(int); // return 0 for ok and -1 for needing extra setting a nextNode from buffer class.
	int AddNeighbor(int,int); // add neighbor & update version
	int IsFull();
	int SearchNeighbors(int); // search neightbors if equal with "int".If exists return 0 else 1
	void CreateTables(int); //dimiourgei tous pinakes neighbor kai edgeProperty
	int ShortestPath(Index*,char , int,int,int,Queue<int>*,int);
	void PrintNeightbors(int,char);
	void PrintNeightborsINC(int);
};

class Buffer {
	Node *incoming; // Dynamic array of graph incoming adjacency list
	int incSize; // Size of incoming array.Needed for realloc
	int incEnd; // first empty cell of array.(The next of the last cell)
	Node *outcoming; // Dynamic array of graph outcoming adjacency list
	int outSize;
	int outEnd;
public:
	Buffer(int);
	~Buffer();
	Node* GetListNode(char);
	int GetIncEnd();
	int GetOutEnd();

	void InsertBuffer(int, int, Index*);
	void InsertBuffer(int, int, Index*,int);
	void IncreaseEndPos(char c);
	void AddNeighbor(int, int, Index*,int version);
	void AddNeighbor(int, int, Index*);
	void AddEdge(int, int, Index*);
	int Query(int , int, Index*,int,int,int);
	int SearchNodeNeighbours(Node*,Index*, char, int,int,int,Queue<int>*,int);
	/*
	* Increase capacity of Buffer data type.
	* if char == 'i' then realloc incoming array
	* else if char == 'o' then realloc outcoming array.
	*/
	void Reallocate(char);
	void PrintBuffer(Index *);
	int Find_First_Unmarked(Index*);
	CC* estimateConnectedComponents(Index*);
	int BFS(Index*,int,int,CC*);
};

#endif /* BUFFER_H_ */
