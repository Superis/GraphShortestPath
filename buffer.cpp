/*
 * Buffer.cpp
 *
 *  Created on: Oct 27, 2016
 *      Author: alex
 */

#include <iostream>
#include <iomanip>
#include <cstring>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "queue.h"

#include "buffer.h"


using namespace std;

ofstream bufOutput("graphOUTCOMING.txt");
ofstream bufincoming("graphINCOMING_REVERSED.txt");

/**************		Node class	 **************/

// Constructor & destructor are never used.
Node::Node() : endPos(0), nextNode(0) {
	maxCapacity = N;
}

Node::~Node() {
	//cout << "A Node has been deleted" << endl;
}

int Node::AddNeighbor(int i) {
	// mpainei stin arxi o elegxos giati an mpei meta to endPos++
	// mporei na min exoume kapion extra geitona , opote spatali mnimis
	if (endPos >= maxCapacity) { // reached max cap
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

int Node::GetNeighbor(int pos) {
	return neighbor[pos];
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

void Node::PrintNeightbors(int src ,char c) {
	if (c == 'o') {
		for (int i = 0; i < endPos; i++) {
			if (neighbor[i] >= 0) {
				bufOutput << setw(8) << std::left << src << neighbor[i] << endl;
			}
		}
	}
	else if (c == 'i'){
		for (int i = 0; i < endPos; i++) {
			if (neighbor[i] >= 0) {
				bufincoming << setw(8) << std::left << neighbor[i] << src << endl;
			}
		}
	}
}

int Node::ShortestPath(Index* index, char direction, int level) {
	int i;
	IndexNode* indArray = index->GetIndexNode();
	if (direction == 's') {
		for (i = 0; i < endPos; i++) {
			if (indArray[neighbor[i]].dest_level >= 0)
				return level + indArray[neighbor[i]].dest_level;
			else if (indArray[neighbor[i]].src_level < 0) {
				indArray[neighbor[i]].src_level = level;
			}
		}
	} else if (direction == 'd') {
		for (i = 0; i < endPos; i++) {
			if (indArray[neighbor[i]].src_level >= 0)
				return level + indArray[neighbor[i]].src_level;
			else if (indArray[neighbor[i]].dest_level < 0) {
				indArray[neighbor[i]].dest_level = level;
			}
		}
	}
	if (i == maxCapacity && nextNode != 0)
		return -nextNode;
	else
		return 0;
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

int Index::GetNeighbor(int target, Buffer* buffer) {
	int cap,pos,endPos,nextNode;
	Node *out = buffer->GetListNode('o');
	endPos = out[indexArray[target].out].GetEndPos();
	pos = indexArray[target].recursive_level;
	cap = out[indexArray[target].out].GetCapacity();
	if (cap == 0) {
		cout << "ZEROOOOOOOOOOOOOO with target : " << target << " @ : " << indexArray[target].out << endl;
		return -1;
	}
	nextNode = out[indexArray[target].out].GetNextNode();
	// Checking where the requested neighbor is located.
	if (pos >= endPos ) {
		if (nextNode == 0) {
			cout << "Wrong neighbor pos :: out of reach!" << endl;
			return -1;
		}
		//iterate through Nodes that contain the rest neighbors
		int count = 0,targetNode;
		targetNode = pos/cap; // = node that the neighbor is located
		nextNode = indexArray[target].out;
		do {
			count++;
			nextNode = buffer->GetListNode('o')[nextNode].GetNextNode();
		} while (count != targetNode);
		int newpos = pos%cap;
		return out[nextNode].GetNeighbor(newpos);
	}
	else // it's located at the first node.
		return out[indexArray[target].out].GetNeighbor(pos);
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
		indexArray[src].outlast = buf->GetOutEnd();
		buf->IncreaseEndPos('o');
	}
	if (indexArray[dest].in == -1) {
		indexArray[dest].in = buf->GetIncEnd();
		indexArray[dest].inlast = buf->GetIncEnd();
		buf->IncreaseEndPos('i');
	}
}

void Index::CheckCap(int src, int dest) {
	if (src >= dest) {
		if (src > this->indSize) {
			cout << "SRC REALLOC" << endl;
			Reallocate(src);
		}
	}
	else {
		if (dest > this->indSize) {
			cout << "DEST REALLOC" << endl;
			Reallocate(dest);
		}
	}
}

 void Index::Reallocate(int newCapacity) {
	 IndexNode *tmp = new IndexNode[indSize + newCapacity + 1];
	 memcpy(tmp,indexArray,indSize * sizeof(IndexNode));
	 delete[] indexArray;
	 indexArray = tmp;
	 indSize = indSize + newCapacity + 1;
 }

int Index::NeighboursNum(int target, char c, Buffer *buf) {
	Node* tmpnode = buf->GetListNode(c);
	int pos;
	if (c == 'o')
		pos = indexArray[target].out;
	else if (c == 'i')
		pos = indexArray[target].in;
	int end;
	int sum = 0;
	if (pos == -1) // no neighbors
		return 0;
	do {
		end = tmpnode[pos].GetEndPos();
		if (end < tmpnode[pos].GetCapacity())
			return sum + end;
		else {
			sum += end;
			pos = tmpnode[pos].GetNextNode();
		}
	} while (pos != 0); // add up the neighbors until there is not NextNode
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
	if (outcoming[indexA[src].outlast].AddNeighbor(dest) == -1) {
		outcoming[indexA[src].outlast].SetNextNode(outEnd);
		indexA[src].outlast = outEnd;
		if (outcoming[indexA[src].outlast].AddNeighbor(dest) == -1)
			cout << "Wrong insert @ outcoming" << endl;
		outEnd++;
	}

	if (incoming[indexA[dest].inlast].AddNeighbor(src) == -1) {
		incoming[indexA[dest].inlast].SetNextNode(incEnd);
		indexA[dest].inlast = incEnd;
		if (incoming[indexA[dest].inlast].AddNeighbor(src) == -1)
			cout << "Wrong insert @ incoming" << endl;
		incEnd++;
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
	int src_pos; //= indArray[src].out;
	Node* src_node; //=&(outcoming[src_pos]);
	int dest_pos; // = indArray[src].in;
	Node* dest_node; // = &(incoming[dest_pos]);
	indArray[src].src_level = 0;
	indArray[dest].dest_level = 0;
	int level = 1;
	int k;
	int counter_s, counter_d;
	if (indArray[src].out == -1 || indArray[dest].in == -1) {
		indArray[src].src_level = -1;
		indArray[dest].dest_level = -1;
		return -1;
	}
	if (index->NeighboursNum(src, 'o', this)
			<= index->NeighboursNum(dest, 'i', this)) {
		while (1) {
			counter_s = 0;
			for (int i = 0; i < l; i++) {
				if (indArray[i].src_level == level - 1) {
					counter_s++;
					src_pos = indArray[i].out;
					if (src_pos == -1)
						continue;
					src_node = &(outcoming[src_pos]);
					k = this->SearchNodeNeighbours(src_node, index, 's', level);
					if (k > 0) {
						for (int p = 0; p < l; p++) {
							indArray[p].src_level = -1;
							indArray[p].dest_level = -1;
						}
						return k;
					} else
						continue;
				}
			}
			if (counter_s == 0)
				break;
			counter_d = 0;
			for (int i = 0; i < l; i++) {
				if (indArray[i].dest_level == level - 1) {
					counter_d++;
					dest_pos = indArray[i].in;
					if (dest_pos == -1)
						continue;
					dest_node = &(incoming[dest_pos]);
					k = this->SearchNodeNeighbours(dest_node, index, 'd',
							level);
					if (k > 0) {
						for (int p = 0; p < l; p++) {
							indArray[p].src_level = -1;
							indArray[p].dest_level = -1;
						}
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
				if (indArray[i].dest_level == level - 1) {
					counter_d++;
					dest_pos = indArray[i].in;
					if (dest_pos == -1)
						continue;
					dest_node = &(incoming[dest_pos]);
					k = this->SearchNodeNeighbours(dest_node, index, 'd',
							level);
					if (k > 0) {
						for (int p = 0; p < l; p++) {
							indArray[p].src_level = -1;
							indArray[p].dest_level = -1;
						}
						return k;
					} else
						continue;
				}
			}
			if (counter_d == 0)
				break;
			counter_s = 0;
			for (int i = 0; i < l; i++) {
				if (indArray[i].src_level == level - 1) {
					counter_s++;
					src_pos = indArray[i].out;
					if (src_pos == -1)
						continue;
					src_node = &(outcoming[src_pos]);
					k = this->SearchNodeNeighbours(src_node, index, 's', level);
					if (k > 0) {
						for (int p = 0; p < l; p++) {
							indArray[p].src_level = -1;
							indArray[p].dest_level = -1;
						}
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
		indArray[p].src_level = -1;
		indArray[p].dest_level = -1;
	}
	return -1;
}

int Buffer::SearchNodeNeighbours(Node* node, Index* index, char c, int level) {
	int k;
	if (c == 's') {
		do {
			k = node->ShortestPath(index, c, level);
			if (k > 0)
				return k;
			else if (k < 0)
				node = &(outcoming[-k]);
			else
				return 0;
		} while (k < 0); //den exei vrei to path akoma
	} else if (c == 'd') {
		do {
			k = node->ShortestPath(index, c, level);
			if (k > 0)
				return k;
			else if (k < 0)
				node = &(incoming[-k]);
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
		if (pos != -1) {
			do {
				outcoming[pos].PrintNeightbors(i,'o');
				pos = outcoming[pos].GetNextNode();
			} while (pos != 0);
		}

		pos = a[i].in;
		if (pos != -1) {
			do {
				incoming[pos].PrintNeightbors(i,'i');
				pos = incoming[pos].GetNextNode();
			} while (pos != 0);
		}
	}
	if (!fork())
		execl("/bin/sh", "sh", "-c", "./insert_unitest.script", (char *) 0);
	wait(NULL);
	cout << "Press a key to proceed" << endl;
	getchar();
	cout << "Continuing..." << endl;
}


//////////////////////////////////////////////////////////////
int Buffer::Find_First_Unmarked(Index* indarr){
	for(int i=0;i<indarr->GetSize();i++){
		if(indarr[i].visited==false)
			return i;
	}
	return -1;
}


int Buffer::Find_Components(Node*S,Index *ind){
	int ccounter=0;
	start_out=ind[Find_First_Unmarked(ind)];
	start_in=ind[Find_First_Unmarked(ind)];
	int nodes_visited=0;
	while(nodes_visited<ind->GetSize()){
		ind[Find_First_Unmarked(ind)].visited=true;
  		//ind[Find_First_Unmarked(ind)].visited=true;
		nodes_visited+=BFS(ind,start_out,start_in,ccounter);
		start_out=ind[Find_First_Unmarked(ind)];
		start_in=ind[Find_First_Unmarked(ind)];
		ccounter++;
	}
}	

int Buffer::BFS(Index*index,int out_position,in_position,component){
	IndexNode*indarr=index->GetIndexNode();
	int outTemp;
	int inTemp;
//	int out_pos=indarr[position].out;
	//int in_pos=indarr[position].in;
//	indarr[position].visited=true;
//	indarr[position].visited=true;
	//bool* visited=new bool(index->GetSize());
	int i;
	//for(i=0;i<index->GetSize();i++
	
//	visited[S]=true;
//	indarr[position].visited=true;
	indarr[position].componentID=component;
	//oura gia ekserxomenous`
	queue Queue_Out(out_position);
	Queue_Out.push_back(out_position);
	queue Queue_In(in_position);
	while(Queue_In.empty==0 || Queue_Out.empty()==0){
		if(Queue_Out.empty()==0){
			
			 outTemp = queue.front();
			 out_position=index[outTemp].out;
        //cout << s << " ";w	
       		 Queue_Out.deque();
 
        // Get all adjacent vertices of the dequeued vertex s
        // If an adjacent has not been visited, then mark it visited
        // and enqueue it
     	   do
     	   {	
			//	outcoming[out_pos].Visift_Neighbors();
     	   		
     	   		for(int i=0;i<this->outcoming[out_position].Get_endPos();i++){
					out_position=outcoming[out_position].GetNextNode();
					neighbor_id=outcoming[out_position].Get_Neighbor(i);
      	    	  if(ind_arr[neighbor_id].visited==false)
     	      	 {
           	   		  ind_arr[neighbor_id].visited = true;
    	          	  Queue_Out.enque(neighbor_id);	
    	          	  indarr[neighbor_id].ComponentID=component;
    	       	 }
    	       }
       	 }while(out_position!=0);
		}
		if(Queue_In.empty()==0){
			inTemp=Queue_In.front();
			 in_position=index[inTemp].in;
			Queue_In.deque();	
			do{
				for(int i=0;i<this->incoming[in_position].Get_endPos();i++){
					inTemp=incoming[inTemp].GetNextNode();
					neighbor_id=incoming[inTemp].Get_Neighbor(i);
      	    	  if(ind_arr[neighbor_id].visited_in==false)
     	      	 {	ind_arr[neighbor_id].visited_in = true;
    	          	Queue_In.enque(neighbor_id);
    	          	indarr[neighbor_id].ComponentID=component;
    	       	 }
				}
			}while(in_position!=0);
		}
	}  
	
	
	
	
//	while()
	
}
