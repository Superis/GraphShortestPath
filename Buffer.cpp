/*
 * Buffer.cpp
 *
 *  Created on: Oct 27, 2016
 *      Author: alex
 */
#include <iostream>

#include "Buffer.h"
#include <stdio.h>


using namespace std;
	/**************		Node class	 **************/

// Constructor & destructor are never used.
Node::Node() : endPos(0),maxCapacity(N),nextNode(0) {
	//cout << "A Node has been created" << endl;
}

Node::~Node() {
	//cout << "A Node has been deleted" << endl;
}

void Node::Init() {
	//endPos = 0;
	maxCapacity = N;
	//nextNode = 0;
}

int Node::AddNeighbor(int i) {
	// mpainei stin arxi o elegxos giati an mpei meta to endPos++
	// mporei na min exoume kapion extra geitona , opote spatali mnimis
	if (endPos >= maxCapacity) {
		cout << "endPos :" << endPos << " max :" << maxCapacity << endl;
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

void Node::PrintNeightbors() {
	cout << flush;
	for (int i = 0 ; i < N ; i++) {
		cout << neighbor[i] << " ";
	}
	cout << endl;
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

void Index::Insert(int src,int dest,Buffer *buf) {
	// Checking if size of Array is enough for the 2 (new) values.If not realloc.
	//this->Print();
	if (src > dest) {
		if (src > indSize - 1) {
			this->Reallocate(src);
		}
	}
	else {
		if (dest  > indSize - 1) {
			this->Reallocate(dest);
		}
	}

	if ( (indexArray[src].initOut == true) && (indexArray[dest].initIn == true) )
		// if initialized from previous Insert(s) do nothing -> return
		return;

	/*
	 * If it doesn't already exists in Index we insert it in the IndexArray[value] cell
	 * then we "link" it with Buffer class by setting the offset values to
	 * be equal with the corresponding cell of buffer arrays.
	 */
	if (indexArray[src].initOut == false) {
		indexArray[src].out = buf->GetOutEnd();
		indexArray[src].initOut = true;
		buf->IncreaseEndPos('o');
	}
	if (indexArray[dest].initIn == false) {
		indexArray[dest].in = buf->GetIncEnd();
		indexArray[dest].initIn = true;
		buf->IncreaseEndPos('i');
	}
}

void Index::Reallocate(int newCapacity) {
	cout << "reallocating index with newCap : " << newCapacity + 1 << " from size :" << indSize << endl;
	indSize = indSize + newCapacity + 1;
	index_node *tmp = (index_node *) realloc(indexArray,indSize * sizeof(index_node));
	if (tmp == NULL) {
	    cout << "indexArray realloc failed!" << endl;
	    exit(EXIT_FAILURE);
	}
	else {
		indexArray = tmp;
	}
}

void Index::Print() {
	for (int i = 0 ; i < indSize ; i++) {
		cout << i << " bool : " << indexArray[i].initIn << " & " << indexArray[i].initOut << endl;
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
	this->InitBuffer('b',0);
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

int Buffer::GetIncEnd() {
	return incEnd;
}

int Buffer::GetOutEnd() {
	return outEnd;
}

void Buffer::InitBuffer(char c,int begin) {
	if (c == 'i') {
		for (int i = begin ; i < incSize ; i++)
			incoming[i].Init();
	}
	else if (c == 'o') {
		for (int i = begin ; i < outSize ; i++)
			outcoming[i].Init();
	}
	else { // an extra condition for Buffer constructor.
		for (int i = begin ; i < incSize ; i++) {
			incoming[i].Init();
			outcoming[i].Init();
		}
	}
}

void Buffer::InsertBuffer(int src, int dest,Index *index) {
	index_node *indexA = index->GetIndexNode();

	if (outEnd >= outSize) { // must realloc
		this->Reallocate('o');
	}
	if (incEnd >= incSize) {
		this->Reallocate('i');
	}
	cout << "SRC adding " << src << " @ "<< indexA[src].out << endl;
	if (outcoming[indexA[src].out].AddNeighbor(dest) == -1) { // need another array cell for storing data for this Node
		int pos = indexA[src].out;
		while (outcoming[pos].GetNextNode() != 0) { // Find the non-full array cell to add neighbor
			pos = outcoming[pos].GetNextNode();
		}
		outcoming[pos].SetNextNode(outEnd);
		outcoming[outEnd].AddNeighbor(dest);
		outEnd++;
	}

	cout << "DEST adding " << dest << " @ "<< indexA[dest].in << endl;
	if (incoming[indexA[dest].in].AddNeighbor(src) == -1) {
		int pos = indexA[dest].in;
		while (incoming[pos].GetNextNode() != 0) {
			pos = incoming[pos].GetNextNode();
		}
		incoming[pos].SetNextNode(incEnd);
		incoming[incEnd].AddNeighbor(src);
		incEnd++;
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

void Buffer::Reallocate(char c) {
	// Thn allaxa elenxontas an einai valid to realloc.Alla tha argei kapws.
	// To poli poli vgazoume ton elenxo an trexei panta komple.
	if (c == 'i') {
		incSize = incSize * 2;
		Node *tmp = (Node*) realloc(incoming,incSize * sizeof(Node));
		if (tmp == NULL) {
		    cout << "Incoming realloc failed!" << endl;
		    exit(EXIT_FAILURE);
		}
		else {
		    incoming = tmp;
			this->InitBuffer('i',incSize/2);

		}
		//incoming = (Node*) realloc(incoming,incSize * sizeof(Node));
	}
	else if (c == 'o') {
		outSize = outSize * 2;
		Node *tmp = (Node*) realloc(outcoming,incSize * sizeof(Node));
		if (tmp == NULL) {
		    cout << "outcoming realloc failed!" << endl;
		    exit(EXIT_FAILURE);
		}
		else {
			outcoming = tmp;
			this->InitBuffer('o',outSize/2);
		}
	}
}

void Buffer::PrintBuffer(Index *index) {
	cout << "Printing incoming array" << endl;
	for (int i = 0 ; i < index->GetSize() ; i++) {
		index_node *a = index->GetIndexNode();
		cout << a[i].out << " pos " << endl;

		cout << "Printing outcoming of " << i << endl;
		//outcoming[i].PrintNeightbors();
		if (outcoming[a[i].out].GetNextNode() == 0) {
			outcoming[i].PrintNeightbors();
		}
		else {
			int pos = a[i].out;
			while (outcoming[pos].GetNextNode() != 0) {
				outcoming[a[i].out].PrintNeightbors();
				pos = outcoming[pos].GetNextNode();
			}
		}

		cout << "Printing incoming of " << i << endl;
		if (incoming[a[i].in].GetNextNode() == 0) {
			incoming[a[i].in].PrintNeightbors();
		}
		else {
			int pos = a[i].in;
			while (incoming[pos].GetNextNode() != 0) {
				incoming[a[i].in].PrintNeightbors();
				pos = incoming[pos].GetNextNode();
			}

		}
	}

}

