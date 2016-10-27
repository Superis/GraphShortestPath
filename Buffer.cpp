/*
 * Buffer.cpp
 *
 *  Created on: Oct 27, 2016
 *      Author: alex
 */

#include "Buffer.h"

Node::Node() : endPos(0),maxCapacity(N),nextNode(-1) { // -1 means there isn't any nextNode.
	//cout << "A Node has been created" << endl
}

Node::~Node() {
	//cout << "A Node has been deleted" << endl
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

Buffer::Buffer() {
	// TODO Auto-generated constructor stub

}

Buffer::~Buffer() {
	//cout << "Buffer has been deleted" << endl
}

