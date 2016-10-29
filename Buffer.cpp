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

int Node::AddNeighbor(int i) {
	// mpainei stin arxi o elegxos giati an mpei meta to endPos++
	// mporei na min exoume kapion extra geitona , opote spatali mnimis
	if (endPos >= maxCapacity) {
		return -1;
	}
	neighbor[endPos] = i;
	endPos++;
	return 0;
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

void Index::Insert(uint32_t src,uint32_t dest,Buffer *buf) {
	// Checking if size of Array is enough for the 2 (new) values.If not realloc.
	if (src > dest) {
		if (src > indSize) {
			indexArray = (index_node*) realloc(indexArray,src * sizeof(index_node*));
		}
	}
	else {
		if (dest > indSize) {
			indexArray = (index_node*) realloc(indexArray,dest * sizeof(index_node*));
		}
	}

	if ( (indexArray[src].init == true) && (indexArray[dest].init == true) )
		// if initialized from previous Insert(s) do nothing -> return
		return;
	else if ( (indexArray[src].init == false) && (indexArray[dest].init == false) ) {
		// an valoume prwta to insert tou index stin main to -1 sta Inc/OutEnd() den xreiazetai
		/*
		 * If it doesn't already exists in Index we insert it in the IndexArray[value] cell
		 * then we "link" it with Buffer class by setting the offset values to
		 * be equal with the corresponding cell of buffer arrays.
		 */

		// SHMEIWSH : edw einai i periptwsi pou kai ta 2 einai kainourgia stoixeia.
		// Auto skeftika twra an exete kati allo peite.Alli lysh einai na pername ena orisma stin Insert pou aplopoiei ton kwdika poli kai apla tin kaloume 2 fores stin main
		// To inc einai -2(proteleutaio stoixeio) giati to teleutaio stoixeio einai tou dest.
		indexArray[src].in = buf->GetIncEnd() - 2;
		indexArray[src].out = buf->GetOutEnd() - 2;
		indexArray[src].init = true;
		indexArray[dest].in = buf->GetIncEnd() - 1;
		indexArray[dest].out = buf->GetOutEnd() - 1;
		indexArray[dest].init = true;
	}
	else if (indexArray[src].init == false) {
		indexArray[src].in = buf->GetIncEnd() - 1;
		indexArray[src].out = buf->GetOutEnd() - 1;
		indexArray[src].init = true;
	}
	else if (indexArray[dest].init == false) {
		indexArray[dest].in = buf->GetIncEnd() - 1;
		indexArray[dest].out = buf->GetOutEnd() - 1;
		indexArray[dest].init = true;
	}
}



	/**************		Buffer class	 **************/

Buffer::Buffer() : incSize(N) , incEnd(0) , outSize(N) , outEnd(0) {
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

uint32_t Buffer::GetIncEnd() {
	return incEnd;
}

uint32_t Buffer::GetOutEnd() {
	return outEnd;
}

int Buffer::InsertBuffer(uint32_t src, uint32_t dest,Index *index) {

	return EXIT_SUCCESS;
}

void Buffer::Increase(char c) {
	if (c == 'i') {
		incoming = (Node*) realloc(incoming, 2 * incSize * sizeof(Node));
	}
	else if (c == 'o') {
		outcoming = (Node*) realloc(outcoming, 2 * outSize * sizeof(Node));
	}
}


