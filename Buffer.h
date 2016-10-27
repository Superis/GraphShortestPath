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

#define N 10

class Node {
	//int id; na to prosthesoume gia extra sigouria?
	uint32_t neighbor[N]; //the ids of the neighbor nodes
	int endPos;
	int maxCapacity; // maxCapacity of arrays|Needed for reallocation.
	uint32_t edgeProperty[N]; //property for each edge
	int nextNode;
public:
	Node();
	~Node();

	void AddNeighbor(int);
	void SetNextNode(int);
};

struct index_node {
	int id;
	int in; // offset of incoming node
	int out; // offset of outcoming node
};

class Index {
public:
	Index();
	~Index();
};

class Buffer {
	Node incoming[N];
	Node outcoming[N];
	index_node indexArray[N];
public:
	Buffer();
	~Buffer();

	Node* GetListNode();
	int DestroyBuffer();
	int InsertBuffer(int,int);
	index_node* GetIndexNode();
};

#endif /* BUFFER_H_ */
