
#include <cstdlib>
#include <stdint.h>

#ifndef BUFFER_H_
#define BUFFER_H_

#define N 20

class Node {
	//int id; na to prosthesoume gia extra sigouria?
	uint32_t neighbor[N]; //the ids of the neighbor nodes
	uint32_t endPos; // pointing to first empty cell of array.
	uint32_t maxCapacity; // maxCapacity of arrays|Needed for reallocation.
	uint32_t edgeProperty[N]; //property for each edge
	uint32_t nextNode;
public:
	Node();
	~Node();
	void Init();

	uint32_t GetNextNode();

	int AddNeighbor(int); // return 0 for ok and -1 for needing extra setting a nextNode from buffer class.
	void SetNextNode(int);

	void PrintNeightbors();
};


struct index_node {
	//int id; // id of corresponding graph Node
	uint32_t in; // offset of incoming node
	uint32_t out; // offset of outcoming node
	bool initIn; // check parameter if node is initialized
	bool initOut;
	index_node() :in(0),out(0),initIn(false),initOut(false) {}
};

class Buffer;

class Index {
	index_node *indexArray; // Dynamic array of graph indices
	uint32_t indSize;
public:
	Index();
	~Index();

	void IndexInit();
	index_node* GetIndexNode();
	uint32_t GetSize() {return indSize;};

	void Insert(uint32_t,uint32_t,Buffer *);
	void Reallocate(int); // increase capacity of Index data type till == "int" | Realloc

	void Print();
};

class Buffer {
	Node *incoming; // Dynamic array of graph incoming adjacency list
	uint32_t incSize; // Size of incoming array.Needed for realloc
	uint32_t incEnd; // first empty cell of array.(The next of the last cell)
	Node *outcoming; // Dynamic array of graph outcoming adjacency list
	uint32_t outSize;
	uint32_t outEnd;
public:
	Buffer();
	~Buffer();

	Node* GetListNode(char);
	uint32_t GetIncEnd();
	uint32_t GetOutEnd();

	void InitBuffer(char , uint32_t); // initializing Buffer starting from "uint32_t"-th cell till maxSize
	void InsertBuffer(uint32_t,uint32_t,Index*);
	void IncreaseEndPos(char c);
	int DestroyBuffer();
	/*
	 * Increase capacity of Buffer data type.
	 * if char == 'i' then realloc incoming array
	 * else if char == 'o' then realloc outcoming array.
	 */
	void Reallocate(char);
	void PrintBuffer(Index *);
};

#endif /* BUFFER_H_ */
