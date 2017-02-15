/*
 * components.cpp
 *
 *  Created on: Dec 1, 2016
 *      Author: alex
 */
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <time.h>

#include "components.h"
#include "template_list.h"

using namespace std;

int min(int a, int b) {
	if (a < b)
		return a;
	else
		return b;
}

SCC::SCC(int _size) :
		componentsCount(0), size(_size), level(0) {
	PushChecker = NULL;
	components = new Component*[_size];
	edges = NULL;
	edgesArray =  NULL;
}

SCC::~SCC() {
	for (int i=0;i<componentsCount;i++){
		//delete components[i]->connectedComponents;
		delete components[i]->includedNodesID;
		delete components[i];
		delete edges[i];
		delete[] edgesArray[i];
	}
	//delete[] PushChecker;
	delete[] components;
	delete[] edges;
	delete[] edgesArray;
}

void SCC::EstimateSCC(Buffer* buffer, Index* index, int max) {
	int indexSize = index->GetSize();
	Stack<int>* stack = new Stack<int>();
	int *level = new int(); // tarjan's level of search | Allocate and initialize () .
	TarzanInfoStruct *tis = new TarzanInfoStruct[max + 1];
	for (int i = 0; i < indexSize; i++) {
		// if Node is undefined : Tarjan
		if (tis[i].index == -1) {
			Tarjan(i, stack, index, buffer, max, tis, level);
		}
	}
	delete level;
	delete[] tis;
	delete stack;
	this->Print();
}

void SCC::Tarjan(int target, Stack<int>* stack, Index* index, Buffer* buffer,
		int max, TarzanInfoStruct tis[], int *level) {
	IndexNode *indArr = index->GetIndexNode();
	tis[target].index = *level;
	tis[target].lowlink = *level;
	(*level)++;
	stack->Push(target);
	tis[target].visited = true;
	tis[target].recursive_level = 0;
	Node *out = buffer->GetListNode('o');
	if (out == NULL) {
		cout << "\tERROR!Tarjan failed" << endl;
		return;
	}
	int neighborSum = 0, child = -1;
	while (1) {
		// DFS
		neighborSum = indArr[target].outNeighbors;
		if (neighborSum > tis[target].recursive_level) {
			child = index->GetNeighbor(target, buffer,
					tis[target].recursive_level); //out[indArr[target].out].GetNeighbor(indArr[target].recursive_*level,target,index,buffer);

			if (child < 0 || child > max) {
				cout << "Tarjan found child with unidentified value :" << child
						<< endl;
				break;
			}
			tis[target].recursive_level++;
			if (tis[child].index == -1) {
				tis[child].index = *level;
				tis[child].lowlink = *level;
				tis[child].parentNode = target;
				tis[child].recursive_level = 0;
				(*level)++;
				stack->Push(child);
				tis[child].visited = true;
				target = child;
			} else if (tis[child].visited == true) {
				tis[target].lowlink = MIN(tis[child].index,
						tis[target].lowlink);
			}
		} else {
			// found a SCC
			if (tis[target].lowlink == tis[target].index) {
				Component* comp = new Component();
				comp->componentID = this->componentsCount;
				int top = stack->Pop();
				if (top == -1) {
					cout << "top(1) pop empty" << endl;
					break;
				}
				indArr[top].componentID = this->componentsCount;
				tis[top].visited = false;
				comp->includedNodesID->Push(top);
				int size = 1;
				while (top != target) {
					top = stack->Pop();
					if (top == -1) {
						cout << "top(2) pop empty" << endl;
						break;
					}
					indArr[top].componentID = this->componentsCount;
					comp->includedNodesID->Push(top);
					tis[top].visited = false;
					size++;
				}
				comp->nodesSum = size;
				this->AddComponentToArray(comp);
			}

			// "recurse" one level backwards
			int newTarget = tis[target].parentNode;
			if (newTarget >= 0) {
				tis[newTarget].lowlink = MIN(tis[newTarget].lowlink,
						tis[target].lowlink);
				target = newTarget;
			} else
				// if root ,break while loop.Finished
				break;
		}
	}
}

void SCC::AddComponentToArray(Component* comp) {
	if (componentsCount < size) {
		components[componentsCount] = comp;
		componentsCount++;
	} else {
		Component** newcomp = new Component*[2 * size];
		for (int i = 0; i < size; i++) {
			newcomp[i] = components[i];
		}
		newcomp[componentsCount] = comp;
		componentsCount++;
		delete[] components;
		components = newcomp;
		size = 2 * size;
	}
}

int SCC::FindNodeSCC_ID(int nodeid, Index* index) {
	return index->GetIndexNode()[nodeid].componentID;
}

bool SCC::DestroySCC() {
	for (int i = 0; i < componentsCount; i++) {
		delete components[i]->includedNodesID;
		delete components[i];
	}
	return true;
}

void SCC::Print() {
	//cout << "Found " << componentsCount << " overall SCC" <<  endl;
	//getchar();
	//for (int i = 0 ; i < componentsCount ; i++) {
	//components[i]->includedNodesID->Print();
	//}
	//Component *temp;
	//while(!components.isEmpty()) {
	//temp = components.PopHead();
	//cout << "Printing component #ID : " << temp->componentID << endl;
	//temp->includedNodesID.Print();
	//}
}

int SCC::SrcBeforeDest(Index* index,Buffer* buffer,int level,int compsrc,int compdest,int src,int dest,int repeat,int threadNum,Queue<int>* src_queue,Queue<int>* dest_queue) {
	IndexNode *indArray = index->GetIndexNode();
	int i,k,src_pos,dest_pos;
	Node *src_node,*dest_node;
	int counter_s = 0,counter_d=0;
	int _size = src_queue->GetSize();
	int count = 0;
	while (count < _size) {
		i = src_queue->Dequeue();
		count++;
		counter_s++;
		src_pos = indArray[i].out;
		if (src_pos == -1){
			counter_s--;
			continue;
		}
		src_node = &(buffer->GetListNode('o')[src_pos]);
		k = buffer->SearchNodeNeighbours(src_node,index, 's','s', level,compsrc,dest,repeat,src_queue,threadNum,this);
		if (k > 0){
			delete src_queue;
			delete dest_queue;
			return k;
		}
		else
			continue;
	}
	if (counter_s == 0){
		delete src_queue;
		delete dest_queue;
		return -1;
	};
	_size = dest_queue->GetSize();
	count = 0;
	while (count < _size) {
		i = dest_queue->Dequeue();
		count++;
		counter_d++;
		dest_pos = indArray[i].in;
		if (dest_pos == -1){
			counter_d--;
			continue;
		}
		dest_node = &(buffer->GetListNode('i')[dest_pos]);
		k = buffer->SearchNodeNeighbours(dest_node,index, 'd','s', level,compdest,src,repeat,dest_queue,threadNum,this);
		if (k > 0){
			delete src_queue;
			delete dest_queue;
			return k;
		}
		else
			continue;
	}
	if (counter_d == 0){
		delete src_queue;
		delete dest_queue;
		return -1;
	}
	return 0;
}


int SCC::DestBeforeSrc(Index* index,Buffer* buffer,int level,int compsrc,int compdest,int src,int dest,int repeat,int threadNum,Queue<int>* src_queue,Queue<int>* dest_queue) {
	IndexNode *indArray = index->GetIndexNode();
	int i,k,src_pos,dest_pos;
	Node *src_node,*dest_node;
	int counter_s = 0,counter_d=0;
	int _size = dest_queue->GetSize();
	int count = 0;
	while (count < _size) {
		i = dest_queue->Dequeue();
		count++;
		counter_d++;
		dest_pos = indArray[i].in;
		if (dest_pos == -1){
			counter_d--;
			continue;
		}
		dest_node = &(buffer->GetListNode('i')[dest_pos]);
		k = buffer->SearchNodeNeighbours(dest_node,index, 'd','d', level,compdest,src,repeat,dest_queue,threadNum,this);
		if (k > 0){
			delete src_queue;
			delete dest_queue;
			return k;
		}
		else
			continue;
	}
	if (counter_d == 0){
		delete src_queue;
		delete dest_queue;
		return -1;
	}
	counter_s = 0;
	_size = src_queue->GetSize();
	count = 0;
	while (count < _size) {
		i = src_queue->Dequeue();
		count++;
		counter_s++;
		src_pos = indArray[i].out;
		if (src_pos == -1){
			counter_s--;
			continue;
		}
		src_node = &(buffer->GetListNode('o')[src_pos]);
		k = buffer->SearchNodeNeighbours(src_node,index, 's','d', level,compsrc,dest,repeat,src_queue,threadNum,this);
		if (k > 0){
			delete src_queue;
			delete dest_queue;
			return k;
		}
		else
			continue;
	}
	if (counter_s == 0){
		delete src_queue;
		delete dest_queue;
		return -1;
	}
	return 0;
}

int SCC::EstimateShortestPathSCC(Buffer* buffer, Index* index, int src,
		int dest, int repeat, int threadNum) {
			IndexNode *indArray = index->GetIndexNode();
			Queue<int>* src_queue = new Queue<int>;
			Queue<int>* dest_queue = new Queue<int>;
			src_queue->Enqueue(src);
			dest_queue->Enqueue(dest);
			indArray[src].src_visited[threadNum] = repeat;
			indArray[dest].dest_visited[threadNum] = repeat;
			int compsrc = indArray[src].componentID;
			int compdest = indArray[dest].componentID;
			int level = 1;
			int u;
			if (indArray[src].outNeighbors <= indArray[dest].inNeighbors){
				do {
					u=this->SrcBeforeDest(index,buffer,level,compsrc,compdest,src,dest,repeat,threadNum,src_queue,dest_queue);
					level++;
				} while(u==0);
				return u;
			}
			else{
				do {
					u=this->DestBeforeSrc(index,buffer,level,compsrc,compdest,src,dest,repeat,threadNum,src_queue,dest_queue);
					level++;
				} while(u==0);
				return u;
			}
}

void SCC::BuildHypergraph(Index* index, Buffer* buffer) {
	PushChecker = new int[componentsCount];
	for (int j = 0; j < componentsCount; j++)
		PushChecker[j] = -1;
	int node_pos, current_component, temp;
	IndexNode* k = index->GetIndexNode();
	Node* G = buffer->GetListNode('o');
	int u;
	this->edges = new List<int>*[componentsCount];
	this->edgesArray = new int*[componentsCount];
	for (int i = 0; i < componentsCount; i++) {
		edges[i] = new List<int>;
		current_component = this->components[i]->componentID;
		//components[i]->includedNodesID.Print();
		components[i]->includedNodesID->ResetCur();
		if (!components[i]->includedNodesID->IsOut()) {
			do {
				//cout << i << endl;
				temp = components[i]->includedNodesID->GetCurData();
				node_pos = k[temp].out;
				if (node_pos != -1) {
					do {
						node_pos = G[node_pos].SearchDiffComponent(
								current_component, this, index);
					} while (node_pos);
				}
			} while (components[i]->includedNodesID->IncCur());
		}
		components[i]->includedNodesID->ResetCur();
		edges[i]->ResetCur();
		u =edges[i]->GetSize();
		this->edgesArray[i]= new int[u];
		for (int j=0;j < u;j++){
			edgesArray[i][j]=edges[i]->GetCurData();
			edges[i]->IncCur();
		}
	}
	delete[] PushChecker;
}

void SCC::ShuffleEdges() {
	srand(time(NULL));
	int k,temp,u;
	for (int i = 0; i < componentsCount; i++) {
		u=edges[i]->GetSize();
		for (int j=0;j < u;j++){
		    k = j + rand() % (u - j);
		    temp = edgesArray[i][j];
		    edgesArray[i][j] = edgesArray[i][k];
		    edgesArray[i][k] = temp;
		}
	}

}

void SCC::BuildGrailIndex(int grailnum) {
	//this->ResetEdges();
	int r = 1;
	if (rand() % 2 ==0) {
		int i = componentsCount - 1;
		while (i >= 0) {
			if (components[i]->label[grailnum].visited == 0) {
				//cout << i << " to i" << endl;
				this->GrailProgress(i, &r,grailnum);
			}
			i--;
		}
	}
	else {
		int i=0;
		while (i<componentsCount){
			if (components[i]->label[grailnum].visited == 0) {
				//cout << i << " to i" << endl;
				this->GrailProgress(i, &r,grailnum);
			}
			i++;
		}
	}
	if (grailnum < GRAILTIMES)
		this->ShuffleEdges();

}

void SCC::GrailProgress(int i, int* r,int grailnum) {
	int min_rank = componentsCount; //timi wste me tin prwti na paroume mikrotero min_rank
	int myrank = *r;
	int new_progress;
	Stack<int> StackProgress;
	StackProgress.Push(i);
	while (!StackProgress.isEmpty()) {
		i = StackProgress.GetHeadData();
		while ((new_progress = this->GetNextArrayEdge(i)) != -1) {
			//somefile << new_progress << " new progresss";
			components[i]->label[grailnum].flag = 1;
			if (components[new_progress]->label[grailnum].visited == 1) {
				min_rank = min(min_rank,
						components[new_progress]->label[grailnum].min_rank);
				continue;
			}
			i = new_progress;
			StackProgress.Push(i);
		}
		components[i]->label[grailnum].rank = myrank;
		if (!components[i]->label[grailnum].flag) {
			components[i]->label[grailnum].min_rank = myrank;
			min_rank = min(min_rank, myrank);
		} else
			components[i]->label[grailnum].min_rank = min_rank;
		//cout << "tha peiraksoume tous visited kai sygkekrimena ton " << i << endl;
		components[i]->label[grailnum].visited = 1;
		myrank++;
		StackProgress.Pop();
	}
	*r = myrank;
}

void SCC::ResetEdges() {
	for (int i = 0; i < componentsCount; i++)
		this->edges[i]->ResetCur();
}

/*int SCC::GetUnvisitedEdge(int i){
 int temp;
 if (!this->edges[i]->IsOut()) {
 do{
 temp = this->edges[i]->GetCurData();
 if (components[temp]->label.visited == 0){
 this->edges[i]->IncCur();
 return temp;
 }
 } while(this->edges[i]->IncCur());
 }
 int k=this->edges[i]->GetHeadData();
 if (k==NULL)
 return -1;
 else
 return -2;
 }*/

int SCC::GetNextEdge(int i) {
	int temp;
	if (!this->edges[i]->IsOut()) {
		temp = this->edges[i]->GetCurData();
		this->edges[i]->IncreaseCur();
		return temp;
	}

	return -1;
}

int SCC::GetNextArrayEdge(int i) {
	int temp;
	if (this->components[i]->lastArrayEdge < this->edges[i]->GetSize()){
		temp = edgesArray[i][this->components[i]->lastArrayEdge];
		this->components[i]->lastArrayEdge++;
		return temp;
	}

	this->components[i]->lastArrayEdge=0;
	return -1;
}

GRAIL_ANSWER SCC::IsReachableGrail(Index* index, int source, int dest) {
	IndexNode* indArray = index->GetIndexNode();
	if (indArray[source].componentID == indArray[dest].componentID)
		return YES;
	else {  // ELEGXOUME TO EURETIRIO GRAIL
		for (int i=0;i<GRAILTIMES;i++){
			if (this->Subset(components[indArray[dest].componentID]->label[i],
				components[indArray[source].componentID]->label[i]))
				continue;
			else
				return NO;
		}
		return MAYBE;
	}
}

int SCC::Subset(Label a, Label b) {
	if ((a.min_rank >= b.min_rank) && (a.rank <= b.rank))
		return 1;
	else
		return 0;
}
