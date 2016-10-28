/*
 * Buffer.h
 *
 *  Created on: Oct 27, 2016
 *      Author: alex
 */

#include <cstdlib>
#include <stdint.h>

#ifndef BUFFER_H_
#define BUFFER_H_

#define N 20

class Node {
	//int id; na to prosthesoume gia extra sigouria?
	uint32_t neighbor[N]; //the ids of the neighbor nodes
	int endPos;
	int maxCapacity; // maxCapacity of neighbor array|Needed for reallocation.
	uint32_t edgeProperty[N]; //property for each edge
	int nextNode;
public:
	Node();
	~Node();

	void AddNeighbor(int);
	void SetNextNode(int);
};

struct index_node {
	int id; // id of corresponding graph Node
	int in; // offset of incoming node
	int out; // offset of outcoming node
};

class Index {
	index_node *indexArray; // Dynamic array of graph indices
	int indSize;
public:
	Index();
	~Index();

	index_node* GetIndexNode();

	void Insert(int,int);
	void Increase(int); // increase capacity of Index data type till == "int" | Realloc
};

class Buffer {
	Node *incoming; // Dynamic array of graph incoming adjacency list
	int incSize; // Size of incoming array.Needed for realloc
	Node *outcoming; // Dynamic array of graph outcoming adjacency list
	int outSize;
public:
	Buffer();
	~Buffer();

	Node* GetListNode(char);

	int DestroyBuffer();
	int InsertBuffer(int,int);
	/*
	 * Increase capacity of Buffer data type.
	 * if char == 'i' then realloc incoming array
	 * else if char == 'o' then realloc outcoming array.
	 */
	void Increase(char);
};

#endif /* BUFFER_H_ */
