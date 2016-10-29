
#include <cstdlib>
#include <stdint.h>

#ifndef BUFFER_H_
#define BUFFER_H_

#define N 20

class Node {
	//int id; na to prosthesoume gia extra sigouria?
	uint32_t neighbor[N]; //the ids of the neighbor nodes
	uint32_t endPos;
	uint32_t maxCapacity; // maxCapacity of arrays|Needed for reallocation.
	uint32_t edgeProperty[N]; //property for each edge
	uint32_t nextNode;
	Node() : endPos(0),maxCapacity(N),nextNode(0) {}
public:
	Node();
	~Node();

	int AddNeighbor(int); // return 'o' for ok and 'r' for setting a nextNode from buffer class.
	void SetNextNode(int);
};

struct index_node {
	//int id; // id of corresponding graph Node
	uint32_t in; // offset of incoming node
	uint32_t out; // offset of outcoming node
	bool init; // check parameter if node is initialized
	index_node() : in(0),out(0),init(false) {}
};

class Buffer;

class Index {
	index_node *indexArray; // Dynamic array of graph indices
	uint32_t indSize;
public:
	Index();
	~Index();

	index_node* GetIndexNode();

	void Insert(uint32_t,uint32_t,Buffer *);
	void Increase(int); // increase capacity of Index data type till == "int" | Realloc
};

class Buffer {
	Node *incoming; // Dynamic array of graph incoming adjacency list
	uint32_t incSize; // Size of incoming array.Needed for realloc
	uint32_t incEnd; // end position of incoming array.
	Node *outcoming; // Dynamic array of graph outcoming adjacency list
	uint32_t outSize;
	uint32_t outEnd;
public:
	Buffer();
	~Buffer();

	Node* GetListNode(char);
	uint32_t GetIncEnd();
	uint32_t GetOutEnd();

	int InsertBuffer(uint32_t,uint32_t,Index*);
	int DestroyBuffer();
	/*
	 * Increase capacity of Buffer data type.
	 * if char == 'i' then realloc incoming array
	 * else if char == 'o' then realloc outcoming array.
	 */
	void Increase(char);
};

#endif /* BUFFER_H_ */
