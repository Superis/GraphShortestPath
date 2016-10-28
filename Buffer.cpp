/*
 * Buffer.cpp
 *
 *  Created on: Oct 27, 2016
 *      Author: alex
 */
#include <iostream>

#include "Buffer.h"

using namespace std;
	/**************		Node class	 **************/

Node::Node() : endPos(0),maxCapacity(N),nextNode(-1) { // -1 means there isn't any nextNode.
	//cout << "A Node has been created" << endl;
}

Node::~Node() {
	//cout << "A Node has been deleted" << endl;
}

void Node::AddNeighbor(int i) {
	neighbor[endPos] = i;
	endPos++;
	if (endPos >= maxCapacity) {
		// REALLOCATE
	}
}

void Node::SetNextNode(int i) {
	nextNode = i;
}



	/**************		Index class 	**************/

Index::Index() : indSize(N) {
	indexArray = (index_node*) malloc(N * sizeof(index_node));
	if (indexArray == NULL) {
		cerr << "Failed to allocate memory for indexArray!" << endl;
		exit(EXIT_FAILURE);
	}
	cout << "An Index-data_type has been created." << endl;
}

Index::~Index() {
	free(indexArray);
	cout << "An Index-data_type has been deleted." << endl;
}

index_node* Index::GetIndexNode() {
	return this->indexArray;
}

void Index::Insert(int sourc,int dest) {

}



	/**************		Buffer class	 **************/

Buffer::Buffer() : incSize(N) , outSize(N) {
	incoming = (Node*) malloc(N * sizeof(Node));
	if (incoming == NULL) {
		cerr << "Failed to allocate memory for incoming array!" << endl;
		exit(EXIT_FAILURE);
	}
	outcoming = (Node*) malloc(N * sizeof(Node));
	if (outcoming == NULL) {
		cerr << "Failed to allocate memory for outcoming array!" << endl;
		exit(EXIT_FAILURE);
	}
	cout << "A buffer-data_type has been created." << endl;
}

Buffer::~Buffer() {
	free(incoming);
	free(outcoming);
	cout << "A buffer-data_type has been deleted" << endl;
}

Node* Buffer::GetListNode(char c) {
	if (c == 'i') {
		return incoming;
	}
	else if (c == 'o') {
		return outcoming;
	}
	cout << "Buffer returning NULL" << endl;
	return NULL;
}

void Buffer::Increase(char c) {
	if (c == 'i') {
		incoming = (Node*) realloc(incoming, 2 * incSize * sizeof(Node));
	}
	else if (c == 'o') {
		outcoming = (Node*) realloc(outcoming, 2 * outSize * sizeof(Node));
	}
}


