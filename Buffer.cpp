/*
 * Buffer.cpp
 *
 *  Created on: Oct 27, 2016
 *      Author: alex
 */
#include <iostream>
#include <cstring>

#include "Buffer.h"

using namespace std;
	/**************		Node class	 **************/

// Constructor & destructor are never used.
Node::Node() : endPos(0),maxCapacity(N),nextNode(0) {}

Node::~Node() {
	//cout << "A Node has been deleted" << endl;
}

int Node::AddNeighbor(int i) {
	// mpainei stin arxi o elegxos giati an mpei meta to endPos++
	// mporei na min exoume kapion extra geitona , opote spatali mnimis
	if (endPos >= maxCapacity) {
		//cout << "endPos :" << endPos << " max :" << maxCapacity << endl;
		return -1;
	}
	neighbor[endPos] = i;
	endPos++;
	return 0;
}

int Node::GetNextNode() {
	return nextNode;
}

void Node::SetNextNode(int i) {
	nextNode = i;
}

int Node::IsFull() {
	if (endPos == maxCapacity)
		return 1;
	else
		return 0;
}

int Node::SearchNeighbors(int dest) {
	for (int i = 0 ; i < endPos ; i++ ) {
		if (neighbor[i] == dest)
			return 0;
	}
	return 1;
}

void Node::PrintNeightbors() {
	cout << flush;
	for (int i = 0 ; i < endPos ; i++) {
		cout << neighbor[i] << " ";
	}
	cout << endl;
}



	/**************		Index class 	**************/

Index::Index(int maxSize) : indSize(maxSize) {
	indexArray = new index_node [maxSize];
	cout << "An Index-data_type has been created." << endl;
}

Index::~Index() {
	delete[] indexArray;
	cout << "An Index-data_type has been deleted." << endl;
}

index_node* Index::GetIndexNode() {
	return this->indexArray;
}

void Index::Insert(int src,int dest,Buffer *buf) {
	if ( (indexArray[src].out != -1) && (indexArray[dest].in != -1) )
		// if initialized from previous Insert(s) do nothing -> return
		return;

	/*
	 * If it doesn't already exists in Index we insert it in the IndexArray[value] cell
	 * then we "link" it with Buffer class by setting the offset values to
	 * be equal with the corresponding cell of buffer arrays.
	 */
	if (indexArray[src].out == -1) {
		indexArray[src].out = buf->GetOutEnd();
		buf->IncreaseEndPos('o');
	}
	if (indexArray[dest].in == -1) {
		indexArray[dest].in = buf->GetIncEnd();
		buf->IncreaseEndPos('i');
	}
}

/*
void Index::Reallocate(int newCapacity) {
	index_node *tmp = new index_node[indSize + newCapacity + 1];
	memcpy(tmp,indexArray,indSize * sizeof(index_node));
	delete[] indexArray;
	indexArray = tmp;
	indSize = indSize + newCapacity + 1;
}*/

void Index::Print() {
	for (int i = 0 ; i < indSize ; i++) {
		cout << i << " in : " << indexArray[i].in << " &  out " << indexArray[i].out << endl;
	}
}



	/**************		Buffer class	 **************/

Buffer::Buffer(int optVal) : incSize(optVal) , incEnd(0) , outSize(optVal) , outEnd(0) {
	incoming = new Node [optVal];
	outcoming = new Node [optVal];
	//this->InitBuffer('b',0);
	cout << "A buffer-data_type has been created." << endl;
}

Buffer::~Buffer() {
	delete[] incoming;
	delete[] outcoming;
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

int Buffer::GetIncEnd() {
	return incEnd;
}

int Buffer::GetOutEnd() {
	return outEnd;
}

void Buffer::InsertBuffer(int src, int dest,Index *index) {
	index_node *indexA = index->GetIndexNode();

	if (outEnd >= outSize) { // must realloc
		this->Reallocate('o');
	}
	if (incEnd >= incSize) {
		this->Reallocate('i');
	}
	//cout << "SRC adding " << src << " @ "<< indexA[src].out << endl;
	if (outcoming[indexA[src].out].AddNeighbor(dest) == -1) { // need another array cell for storing data for this Node
		int pos = indexA[src].out;
		while (outcoming[pos].GetNextNode() != 0) { // Find the non-full array cell to add neighbor
			pos = outcoming[pos].GetNextNode();
		}
		if (outcoming[pos].IsFull()) { // if current pos full "allocate" the first available array cell
			outcoming[pos].SetNextNode(outEnd);
			outcoming[outEnd].AddNeighbor(dest);
			outEnd++;
		}
		else {
			outcoming[pos].AddNeighbor(dest);
		}
	}

	//cout << "DEST adding " << dest << " @ "<< indexA[dest].in << endl;
	if (incoming[indexA[dest].in].AddNeighbor(src) == -1) {
		int pos = indexA[dest].in;
		while (incoming[pos].GetNextNode() != 0) {
			pos = incoming[pos].GetNextNode();
		}
		if (incoming[pos].IsFull()) {
			incoming[pos].SetNextNode(incEnd);
			incoming[incEnd].AddNeighbor(src);
			incEnd++;
		}
		else {
			incoming[pos].AddNeighbor(src);
		}
	}
}

void Buffer::IncreaseEndPos(char c) {
	if (c == 'i') {
		incEnd++;
	}
	else if (c == 'o')
		outEnd++;
	else {
		incEnd++;
		outEnd++;
	}
}

void Buffer::AddNeighbor(int src,int dest,Index *index) {
	index_node *indArray = index->GetIndexNode();
	int pos = indArray[src].out;
	do {
		if (outcoming[pos].SearchNeighbors(dest) == 0) {
			cout << "A :: Source node : " << src << " has already " << dest << " as neighbor" << endl;
			return;
		}
		pos = outcoming[pos].GetNextNode();
	} while (outcoming[pos].GetNextNode() != 0);
	outcoming[pos].AddNeighbor(dest);
}

void Buffer::Query(int src,int dest,Index *index) {
	index_node *indArray = index->GetIndexNode();

}

void Buffer::Reallocate(char c) {
	if (c == 'i') {
		Node *tmp = new Node[2*incSize];
		memcpy(tmp,incoming,incSize * sizeof(Node));
		delete[] incoming;
		incoming = tmp;
		incSize = 2 * incSize;
	}
	else if (c == 'o') {
		Node *tmp = new Node[2*outSize];
		memcpy(tmp,outcoming,outSize * sizeof(Node));
		delete[] outcoming;
		outcoming = tmp;
		outSize = 2 * outSize;
	}
}

void Buffer::PrintBuffer(Index *index) {
	//index->Print();
	index_node *a = index->GetIndexNode();
	int size = index->GetSize();
	for (int i = 0 ; i < size ; i++) {
		int pos = a[i].out;
		if ( (pos == -1) && (i != 0) )
			break; // break if we reached the end of array.
		cout << "Printing outcoming of " << i << endl;
		//outcoming[i].PrintNeightbors();
		do {
			outcoming[pos].PrintNeightbors();
			pos = outcoming[pos].GetNextNode();
		} while (outcoming[pos].GetNextNode() != 0);
		outcoming[pos].PrintNeightbors();

		pos = a[i].in;
		cout << "Printing incoming of " << i << endl;
		do {
			incoming[pos].PrintNeightbors();
			pos = incoming[pos].GetNextNode();
		} while (incoming[pos].GetNextNode() != 0);
		incoming[pos].PrintNeightbors();

	}
	/*cout << "Printing outcoming array" << endl;
	for (int i = 0 ; i < outEnd ; i ++ ) {
		outcoming[i].PrintNeightbors();
	}
	cout << "Printing incoming array" << endl;
	for (int i = 0 ; i < incEnd ; i ++ ) {
		incoming[i].PrintNeightbors();
	}*/

}

