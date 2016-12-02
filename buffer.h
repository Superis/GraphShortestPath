#ifndef BUFFER_H_
#define BUFFER_H_

#include <cstdlib>
#include <stdint.h>

#define N 5

struct IndexNode {
	int in; // offset of incoming node
	int out; // offset of outcoming node
	int src_level;
	int dest_level;
	IndexNode() :in(-1), out(-1),src_level(-1),dest_level(-1) {}
};

class Buffer;

class Index {
	IndexNode *indexArray; // Dynamic array of graph indices
	int indSize;
public:
	Index(int);
	~Index();

	IndexNode* GetIndexNode();
	int GetSize() { return indSize; };
	int NeighboursNum(int, char, Buffer*);
	void Insert(int, int, Buffer *);
	void Reallocate(int); // increase capacity of Index data type till == "int" | Realloc

	void Print();
};

class Node {
	int id;
	int neighbor[N]; //the ids of the neighbor nodes
	int endPos; // pointing to first empty cell of array.
	int maxCapacity; // maxCapacity of arrays|Needed for reallocation.
	int edgeProperty[N]; //property for each edge
	int nextNode;
public:
	Node();
	~Node();
	void SetID(int ID) { id = ID; };
	int GetNextNode();

	int AddNeighbor(int); // return 0 for ok and -1 for needing extra setting a nextNode from buffer class.
	void SetNextNode(int);
	int IsFull();
	int SearchNeighbors(int); // search neightbors if equal with "int".If exists return 0 else 1
	void CreateTables(int); //dimiourgei tous pinakes neighbor kai edgeProperty
	int Get_MaxCap();
	int Get_endPos();
	int ShortestPath(Index*, char, int);
	void PrintNeightbors(int);
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
	void IncreaseEndPos(char c);
	void AddNeighbor(int, int, Index*);
	int Query(int, int, Index*);
	int SearchNodeNeighbours(Node*,Index*, char, int);
	/*
	* Increase capacity of Buffer data type.
	* if char == 'i' then realloc incoming array
	* else if char == 'o' then realloc outcoming array.
	*/
	void Reallocate(char);
	void PrintBuffer(Index *);
};

#endif /* BUFFER_H_ */
