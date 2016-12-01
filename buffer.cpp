/*
 * Buffer.cpp
 *
 *  Created on: Oct 27, 2016
 *      Author: alex
 */

#include <iostream>
#include <cstring>
#include <stdlib.h>
#include <stdio.h>

#include "buffer.h"

using namespace std;

/**************		Node class	 **************/

// Constructor & destructor are never used.
Node::Node() :
		endPos(0), nextNode(0) {
	maxCapacity = N;
}

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
	if (endPos >= maxCapacity)
		return 1;
	else
		return 0;
}

int Node::SearchNeighbors(int dest) {
	for (int i = 0; i < endPos; i++) {
		if (neighbor[i] == dest)
			return 0;
	}
	return 1;
}

void Node::PrintNeightbors() {
	cout << flush;
	for (int i = 0; i < endPos; i++) {
		cout << neighbor[i] << " ";
	}
	cout << endl;
}

int Node::ShortestPath(int** self_visited, int** opp_visited, int level) {
	int i;
	for (i = 0; i < endPos; i++) {
		if (opp_visited[neighbor[i]][0] == 1)
			//cout << level << "+" << opp_visited[i][1] << endl;
			return level + opp_visited[neighbor[i]][1];
		else if (self_visited[neighbor[i]][0] != 1) {
			self_visited[neighbor[i]][1] = level;
			self_visited[neighbor[i]][0] = 1;
			//cout << "Node" << neighbor[i]<<"visited on level " << level<<endl;
		}
	}
	if (i == maxCapacity && nextNode != 0)
		return -nextNode;
	else
		return 0;
}

int Node::Get_MaxCap() {
	return maxCapacity;
}

int Node::Get_endPos() {
	return endPos;
}

/**************		Index class 	**************/

Index::Index(int maxSize) :
		indSize(maxSize) {
	indexArray = new IndexNode[maxSize];
	cout << "An Index-data_type has been created." << endl;
}

Index::~Index() {
	delete[] indexArray;
	cout << "An Index-data_type has been deleted." << endl;
}

IndexNode* Index::GetIndexNode() {
	return this->indexArray;
}

void Index::Insert(int src, int dest, Buffer *buf) {
	if ((indexArray[src].out != -1) && (indexArray[dest].in != -1))
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
 IndexNode *tmp = new IndexNode[indSize + newCapacity + 1];
 memcpy(tmp,indexArray,indSize * sizeof(IndexNode));
 delete[] indexArray;
 indexArray = tmp;
 indSize = indSize + newCapacity + 1;
 }*/

int Index::NeighboursNum(int target, char c, Buffer *buf) {
	Node* tmpnode = buf->GetListNode(c);
	int pos;
	if (c == 'o')
		pos = indexArray[target].out;
	else if (c == 'i')
		pos = indexArray[target].in;
	int end;
	int sum = 0;
	do {
		end = tmpnode[pos].Get_endPos();
		if (end < tmpnode[pos].Get_MaxCap())
			return sum + end;
		else {
			sum += end;
			pos = tmpnode[pos].GetNextNode();
		}
	} while (pos != 0);
	return sum;
}

void Index::Print() {
	for (int i = 0; i < indSize; i++) {
		cout << i << " in : " << indexArray[i].in << " &  out "
				<< indexArray[i].out << endl;
	}
}

/**************		Buffer class	 **************/

Buffer::Buffer(int maxVal) :
		incSize(maxVal), incEnd(0), outSize(maxVal), outEnd(0) {
	incoming = new Node[maxVal];
	outcoming = new Node[maxVal];
	cout << "A buffer-data_type has been created." << endl;
}

Buffer::~Buffer() {
	cout << "A buffer-data_type has been deleted" << endl;
	delete[] outcoming;
	delete[] incoming;
}

Node* Buffer::GetListNode(char c) {
	if (c == 'i') {
		return incoming;
	} else if (c == 'o') {
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

void Buffer::InsertBuffer(int src, int dest, Index *index) {
	IndexNode *indexA = index->GetIndexNode();

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
			if (outcoming[outEnd].AddNeighbor(dest) == -1) {
				cout << "cannot add neighbor" << endl;
			}
			outEnd++;
		} else {
			if (outcoming[pos].AddNeighbor(dest) == -1) {
				cout << "wrong positi[wkl;uion" << endl;
			}
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
		} else {
			incoming[pos].AddNeighbor(src);
		}
	}
}

void Buffer::IncreaseEndPos(char c) {
	if (c == 'i') {
		incEnd++;
	} else if (c == 'o')
		outEnd++;
	else {
		incEnd++;
		outEnd++;
	}
}

void Buffer::AddNeighbor(int src, int dest, Index *index) {
	IndexNode *indArray = index->GetIndexNode();
	int pos = indArray[src].out;
	do {
		if (outcoming[pos].SearchNeighbors(dest) == 0) {
			cout << "A :: Source node : " << src << " has already " << dest
					<< " as neighbor" << endl;
			return;
		}
		pos = outcoming[pos].GetNextNode();
	} while (pos != 0);
	this->InsertBuffer(src, dest, index);
}

int Buffer::Query(int src, int dest, Index *index) {
	IndexNode *indArray = index->GetIndexNode();
	int l = index->GetSize();
	int** src_visited = new int*[l];
	int** dest_visited = new int*[l];
	for (int i = 0; i < l; i++) {
		src_visited[i] = new int[2];
		dest_visited[i] = new int[2];  //we use 2d-array to show the level
	}
	for (int i = 0; i < l; i++) {
		for (int j = 0; j < 2; j++) {
			src_visited[i][j] = -1;
			dest_visited[i][j] = -1;
		}
	}
	int src_pos;  //= indArray[src].out;
	Node* src_node;				//=&(outcoming[src_pos]);
	int dest_pos;				// = indArray[src].in;
	Node* dest_node;				// = &(incoming[dest_pos]);
	src_visited[src][0] = 1;
	src_visited[src][1] = 0;
	dest_visited[dest][0] = 1;
	dest_visited[dest][1] = 0;
	int level = 1;
	int k;
	int counter_s, counter_d;
	if (indArray[src].out == -1 || indArray[dest].in == -1) {
		for (int p = 0; p < l; p++) {
			delete[] src_visited[p];
			delete[] dest_visited[p];
		}
		delete[] src_visited;
		delete[] dest_visited;
		return -1;
	}
	if (index->NeighboursNum(src, 'o', this)
			<= index->NeighboursNum(dest, 'i', this)) {
		while (1) {
			counter_s = 0;
			for (int i = 0; i < l; i++) {
				if (src_visited[i][1] == level - 1) {
					counter_s++;
					src_pos = indArray[i].out;
					if (src_pos == -1) {
						for (int p = 0; p < l; p++) {
							delete[] src_visited[p];
							delete[] dest_visited[p];
						}
						delete[] src_visited;
						delete[] dest_visited;
						return -1;
					}
					src_node = &(outcoming[src_pos]);
					k = this->SearchNodeNeighbours(src_node, 's', src_visited,
							dest_visited, level);
					if (k > 0) {
						for (int p = 0; p < l; p++) {
							delete[] src_visited[p];
							delete[] dest_visited[p];
						}
						delete[] src_visited;
						delete[] dest_visited;
						return k;
					} else
						continue;
				}
			}
			if (counter_s == 0)
				break;
			counter_d = 0;
			for (int i = 0; i < l; i++) {
				if (dest_visited[i][1] == level - 1) {
					counter_d++;
					dest_pos = indArray[i].in;
					if (dest_pos == -1) {
						for (int p = 0; p < l; p++) {
							delete[] src_visited[p];
							delete[] dest_visited[p];
						}
						delete[] src_visited;
						delete[] dest_visited;
						return -1;
					}
					dest_node = &(incoming[dest_pos]);
					k = this->SearchNodeNeighbours(dest_node, 'd', dest_visited,
							src_visited, level);
					if (k > 0) {
						for (int p = 0; p < l; p++) {
							delete[] src_visited[p];
							delete[] dest_visited[p];
						}
						delete[] src_visited;
						delete[] dest_visited;
						return k;
					} else
						continue;
				}
			}
			if (counter_d == 0)
				break;
			level++;
		}
	} else {
		while (1) {
			counter_d = 0;
			for (int i = 0; i < l; i++) {
				if (dest_visited[i][1] == level - 1) {
					counter_d++;
					dest_pos = indArray[i].in;
					if (dest_pos == -1) {
						for (int p = 0; p < l; p++) {
							delete[] src_visited[p];
							delete[] dest_visited[p];
						}
						delete[] src_visited;
						delete[] dest_visited;
						return -1;
					}
					dest_node = &(incoming[dest_pos]);
					k = this->SearchNodeNeighbours(dest_node, 'd', dest_visited,
							src_visited, level);
					if (k > 0) {
						for (int p = 0; p < l; p++) {
							delete[] src_visited[p];
							delete[] dest_visited[p];
						}
						delete[] src_visited;
						delete[] dest_visited;
						return k;
					} else
						continue;
				}
			}
			if (counter_d == 0)
				break;
			counter_s = 0;
			for (int i = 0; i < l; i++) {
				if (src_visited[i][1] == level - 1) {
					counter_s++;
					src_pos = indArray[i].out;
					if (src_pos == -1) {
						for (int p = 0; p < l; p++) {
							delete[] src_visited[p];
							delete[] dest_visited[p];
						}
						delete[] src_visited;
						delete[] dest_visited;
						return -1;
					}
					src_node = &(outcoming[src_pos]);
					k = this->SearchNodeNeighbours(src_node, 's', src_visited,
							dest_visited, level);
					if (k > 0) {
						for (int p = 0; p < l; p++) {
							delete[] src_visited[p];
							delete[] dest_visited[p];
						}
						delete[] src_visited;
						delete[] dest_visited;
						return k;
					} else
						continue;
				}
			}
			if (counter_s == 0)
				break;
			level++;
		}
	}
	for (int p = 0; p < l; p++) {
		delete[] src_visited[p];
		delete[] dest_visited[p];
	}
	delete[] src_visited;
	delete[] dest_visited;
	return -1;
}

int Buffer::SearchNodeNeighbours(Node* node, char c, int** self, int** opp,
		int level) {
	int k;
	if (c == 's') {
		do {
			k = node->ShortestPath(self, opp, level);
			if (k > 0)
				return k;
			else if (k < 0)
				node = &(outcoming[-k]);
			//src_node->ShortestPath(src_visited, dest_visited, level);
			else
				return 0;
		} while (k < 0); //den exei vrei to path akoma
	} else if (c == 'd') {
		do {
			k = node->ShortestPath(self, opp, level);
			if (k > 0)
				return k;
			else if (k < 0)
				node = &(incoming[-k]);
			//src_node->ShortestPath(src_visited, dest_visited, level);
			else
				return 0;
		} while (k < 0);
	}
	return 0;
}

void Buffer::Reallocate(char c) {
	if (c == 'i') {
		Node *tmp = new Node[2 * incSize];

		memcpy(tmp, incoming, incSize * sizeof(Node));
		delete[] incoming;
		incoming = tmp;
		incSize = 2 * incSize;
	} else if (c == 'o') {
		Node *tmp = new Node[2 * outSize];
		memcpy(tmp, outcoming, outSize * sizeof(Node));
		delete[] outcoming;
		outcoming = tmp;
		outSize = 2 * outSize;
	}
}

void Buffer::PrintBuffer(Index *index) {
	IndexNode *a = index->GetIndexNode();
	int size = index->GetSize();
	for (int i = 0; i < size; i++) {
		int pos = a[i].out;
		if ((pos == -1))
			break; // break if we reached the end of array.
		cout << "Printing outcoming of " << i << endl;
		do {
			outcoming[pos].PrintNeightbors();
			pos = outcoming[pos].GetNextNode();
		} while (pos != 0);

		pos = a[i].in;
		cout << "Printing incoming of " << i << endl;
		do {
			incoming[pos].PrintNeightbors();
			pos = incoming[pos].GetNextNode();
		} while (pos != 0);
	}
}

void Buffer::Destroy() {
	delete[] outcoming;
	delete[] incoming;
}

/*
void Buffer::PrintBuffer(Index *index) {
	//index->Print();
	IndexNode *a = index->GetIndexNode();
	int size = index->GetSize();
	int printed[2 * size] = { 0 }; //pinakas gia na krataei poioi komvoi exoun ektypwthei idi
	for (int i = 0; i < size; i++) {
		int pos = a[i].out;
		if ((pos == -1) && (i != 0))
			break; // break if we reached the end of array.
		cout << "Printing outcoming of " << i << endl;
		//outcoming[i].PrintNeightbors();
		do {
			outcoming[pos].PrintNeightbors();
			pos = outcoming[pos].GetNextNode();
		} while (outcoming[pos].GetNextNode() != 0);
		if (pos) //gia na min ektypwnoume tous komvous tou 0 diples fores
			outcoming[pos].PrintNeightbors();

		pos = a[i].in;
		cout << "Printing incoming of " << i << endl;
		//	incoming[i].PrintNeightbors();
		//printed[i]=1;
		do {
			if (printed[pos] == 0 && pos >= i) {

				incoming[pos].PrintNeightbors();
				printed[pos] = 1;

			}
			pos = incoming[pos].GetNextNode();
		} while (incoming[pos].GetNextNode() != 0);
		if (pos) { //prepei na to ksanadw
			if (!printed[pos]) {
				incoming[pos].PrintNeightbors();
				printed[pos] = 1;
			}
		}
	}

}
*/
