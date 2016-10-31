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
Node::Node() : endPos(0),maxCapacity(N),nextNode(0) {
	//for (int i = 0 ; i < N ; i++)
	//	neighbor[i] = -1;
//	cout << "A Node has been created" << endl;
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

void Node::PrintNeightbors() {
	cout << flush;
	for (int i = 0 ; i < endPos ; i++) {
		cout << neighbor[i] << " ";
	}
	cout << endl;
}



	/**************		Index class 	**************/

Index::Index() : indSize(N) {
	indexArray = new index_node [N];
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

void Index::Reallocate(int newCapacity) {
	index_node *tmp = new index_node[indSize + newCapacity + 1];
	memcpy(tmp,indexArray,indSize * sizeof(index_node));
	delete[] indexArray;
	indexArray = tmp;
	indSize = indSize + newCapacity + 1;
}

void Index::Print() {
	for (int i = 0 ; i < indSize ; i++) {
		cout << i << " in : " << indexArray[i].in << " &  out " << indexArray[i].out << endl;
	}
}



	/**************		Buffer class	 **************/

Buffer::Buffer() : incSize(N) , incEnd(0) , outSize(N) , outEnd(0) {
	incoming = new Node [N];
	outcoming = new Node [N];
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
			incoming[incEnd].AddNeighbor(dest);
			incEnd++;
		}
		else {
			incoming[pos].AddNeighbor(dest);
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

void Buffer::Reallocate(char c) {
	// Thn allaxa elenxontas an einai valid to realloc.Alla tha argei kapws.
	// To poli poli vgazoume ton elenxo an trexei panta komple.
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
	for (int i = 0 ; i < index->GetSize() ; i++) {
		int pos = a[i].out;
		if ( (pos == -1) && (i != 0) )
			break; // break if we reached the end of array.
		cout << "Printing outcoming of " << i << endl;
		//outcoming[i].PrintNeightbors();
		do {
			cout << pos << " pos " << endl;
			outcoming[pos].PrintNeightbors();
			pos = outcoming[pos].GetNextNode();
		} while (outcoming[pos].GetNextNode() != 0);
		pos = a[i].in;
		cout << "Printing incoming of " << i << endl;
		do {
			cout << pos << " pos " << endl;
			incoming[pos].PrintNeightbors();
			pos = incoming[pos].GetNextNode();
		} while (incoming[pos].GetNextNode() != 0);

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

