/*
 * components.cpp
 *
 *  Created on: Dec 1, 2016
 *      Author: alex
 */
#include <stdio.h>
#include <fstream>
#include <sstream>

#include "components.h"

using namespace std;

int min(int a,int b){
	if (a<b)
		return a;
	else
		return b;
}

SCC::SCC(int _size) : componentsCount(0) ,size(_size) {
	components = new Component*[_size];
	edges = NULL;
}

SCC::~SCC() {
	for (int i=0;i<componentsCount;i++){
		delete components[i];
		//delete edges[i];
	}
	delete[] components;
	//delete[] edges;
}

void SCC::EstimateSCC(Buffer* buffer, Index* index ,int max) {
	int indexSize = index->GetSize();
	Stack<int>* stack = new Stack<int>();
	int *level = new int(); // tarjan's level of search | Allocate and initialize () .
	TarzanInfoStruct *tis = new TarzanInfoStruct[max + 1];
	for (int i = 0 ; i < indexSize ; i++) {
		// if Node is undefined : Tarjan
		if (tis[i].index == -1) {
			Tarjan(i,stack,index,buffer,max,tis,level);
		}
	}
	delete level;
	delete[] tis;
	delete stack;
	this->Print();
}

void SCC::Tarjan(int target,Stack<int>* stack,Index* index,Buffer* buffer,int max ,TarzanInfoStruct tis[],int *level) {
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
	int neighborSum = 0 ,child = -1;
	while(1) {
		// DFS
		neighborSum = indArr[target].outNeighbors;
		if ( neighborSum > tis[target].recursive_level ) {
			child = index->GetNeighbor(target,buffer,tis[target].recursive_level);//out[indArr[target].out].GetNeighbor(indArr[target].recursive_*level,target,index,buffer);

			if (child < 0 || child > max) {
				cout << "Tarjan found child with unidentified value :" << child << endl;
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
				tis[target].lowlink = MIN(tis[child].index,tis[target].lowlink);
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

int SCC::FindNodeSCC_ID(int nodeid ,Index* index) {
	return index->GetIndexNode()[nodeid].componentID;
}

bool SCC::DestroySCC() {
	for (int i = 0 ; i < componentsCount ; i++) {
		delete components[i]->includedNodesID;
		delete components[i];
	}
	return true;
}

void SCC::Print() {
	cout << "Found " << componentsCount << " overall SCC" <<  endl;
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



/*int SCC::EstimateShortestPathSCC(Buffer* buffer,Index* index,int compsrc_arg,int compdest_arg,int src ,int dest){
	index_node *indArray = index->GetIndexNode();
	int l=index->GetSize();
	int src_pos;//= indArray[src].out;
				//cout <<"pos"<< src_pos << endl;
	Node* src_node;//=&(outcoming[src_pos]);
	int dest_pos;// = indArray[src].in;
	Node* dest_node;// = &(incoming[dest_pos]);
	indArray[src].src_level = 0;
	indArray[dest].dest_level = 0;
	compsrc=compsrc_arg;
	compdest=compdest_arg;
	int level = 1;
	int k;
	int counter_s, counter_d;
	if (indArray[src].out == -1 || indArray[dest].in == -1){
		indArray[src].src_level = -1;
		indArray[dest].dest_level = -1;
		return -1;
		}
	if (index->NeighboursNum(src, 'o', this) <= index->NeighboursNum(dest, 'i', this)) {
		//cout << "pame source" <<endl;
		while (1) {
			counter_s = 0;
			for (int i = 0; i < l; i++) {
				if (indArray[i].src_level == level - 1) {
					counter_s++;
					if (indArray[i].componentID != compsrc)
					src_pos = indArray[i].out;
					if (src_pos == -1)
						counter_s--;
						continue;
					}
					src_node = &(buffer->outcoming[src_pos]);
					k = buffer->SearchNodeNeighbours(src_node,index, 's', level,-1);
					if (k > 0) {
						for (int p = 0; p<l; p++) {
							indArray[p].src_level=-1;
							indArray[p].dest_level=-1;}
						return k;
					}
					else
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
					if (dest_pos == -1){
						counter_d--;
						continue;
					}
					dest_node = &(incoming[dest_pos]);
					k = this->SearchNodeNeighbours(dest_node,index, 'd', level,compID);
					if (k > 0) {
						for (int p = 0; p<l; p++) {
							indArray[p].src_level=-1;
							indArray[p].dest_level=-1;}
						return k;
					}
					else
						continue;
				}
			}
			if (counter_d == 0)
				break;
			level++;
		}
	}
	else {
		//cout << "pame sto dest" << endl;
		while (1) {
			counter_d = 0;
			for (int i = 0; i < l; i++) {
				if (indArray[i].dest_level == level - 1) {
					counter_d++;
					dest_pos = indArray[i].in;
					if (dest_pos == -1){
						counter_d--;
						continue;
					}
					dest_node = &(incoming[dest_pos]);
					k = this->SearchNodeNeighbours(dest_node,index, 'd', level,compID);
					if (k > 0) {
						for (int p = 0; p<l; p++) {
							indArray[p].src_level=-1;
							indArray[p].dest_level=-1;}
						return k;
					}
					else
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
					if (src_pos == -1){
						counter_s--;
						continue;
					}
					src_node = &(outcoming[src_pos]);
					k = this->SearchNodeNeighbours(src_node,index, 's', level,compID);
					if (k > 0) {
						for (int p = 0; p<l; p++) {
							indArray[p].src_level=-1;
							indArray[p].dest_level=-1;}
						return k;
					}
					else
						continue;
				}
			}
			if (counter_s == 0)
				break;
			level++;
		}
	}
	for (int p = 0; p<l; p++) {
		indArray[p].src_level=-1;
		indArray[p].dest_level=-1;}
	return -1;


}*/

void SCC::BuildHypergraph(Index* index, Buffer* buffer) {
	int node_pos, current_component, temp;
	IndexNode* k = index->GetIndexNode();
	Node* G = buffer->GetListNode('o');
	this->edges = new List<int>*[componentsCount];
	for (int i = 0; i < componentsCount; i++) {
		edges[i] = new List<int>;
		current_component = this->components[i]->componentID;
		//components[i]->includedNodesID.Print();
		components[i]->includedNodesID->ResetCur();
		while (!components[i]->includedNodesID->IsOut()) {
			cout << i << endl;
			temp = components[i]->includedNodesID->GetCurData();
			node_pos = k[temp].out;
			if (node_pos != -1)
				do {
					node_pos = G[node_pos].SearchDiffComponent(
							current_component, this, index);
				} while (node_pos);

			if (!components[i]->includedNodesID->IncCur()) {
				cout << "NEXT IS NULL" << endl;
				break;
			}
		}
		components[i]->includedNodesID->ResetCur();
	}
}

void SCC::BuildGrailIndex() {
	//srand(time(NULL));
	int visit_number = 0;
	int** visited = new int*[componentsCount];
	for (int p = 0; p < componentsCount; p++) {
		visited[p] = new int;
		(*(visited[p])) = 0;
	}
	int r = 1;
	int i;
	//int k=rand() % componentsCount;
	while (visit_number < componentsCount) {
		for (i = 0; i < componentsCount; i++)
			if ((*(visited[i])) == 0)
				break;
		this->GrailProgress(i, visited, &r, &visit_number);
		for (i = 0; i < componentsCount; i++)
			cout << (*(visited[i])) << endl;
	}
}

void SCC::GrailProgress(int i, int** visited, int* r, int* num) {
	int min_rank = 2; //timi wste me tin prwti na paroume mikrotero min_rank
	int myrank = *r;
	int mynum = *num;
	//int flag[componentsCount] = { 0 }; compiler ERROR.
	int *flag = new int[componentsCount];
	for (int j = 0 ; j < componentsCount ; j++)
		flag[j] = 0;
	int new_progress;
	Stack<int> StackProgress;
	StackProgress.Push(i);
	ofstream somefile("some.txt");
	//i=StackProgress.Pop();
	while (!StackProgress.isEmpty()) {
		//i=StackProgress.Pop();
		if ((*(visited[i])) == 1) {
			min_rank = min(min_rank, components[i]->label.min_rank);
			StackProgress.Pop();
			continue;
		}
		while ((new_progress = this->edges[i]->GetUnvisitedEdge(visited)) != -1) {
			cout << new_progress << " new progresss" << endl;
			flag[i] = 1;
			i = new_progress;
			StackProgress.Push(i);
		}
		//cout << i << " to i" << endl;
		somefile << myrank << "to r" << endl;
		//cout  << componentsCount << " components with  array size " << size  << endl;

		components[i]->label.rank = myrank;
		if (!flag[i]) {
			components[i]->label.min_rank = myrank;
			min_rank = min(min_rank, myrank);
		} else
			components[i]->label.min_rank = min_rank;
		(*(visited[i])) = 1;
		mynum++;
		myrank++;
		StackProgress.Pop();
	}
	r = &myrank;
	num = &mynum;
	delete[] flag;

}

GRAIL_ANSWER SCC::IsReachableGrail(Index* index, int source, int dest) {
	IndexNode* indArray = index->GetIndexNode();
	if (indArray[source].componentID == indArray[dest].componentID)
		return YES;
	else {  // ELEGXOUME TO EURETIRIO GRAIL
		if (this->Subset(components[indArray[dest].componentID]->label,
				components[indArray[source].componentID]->label))
			return MAYBE;
		else
			return NO;
	}
}

int SCC::Subset(Label a, Label b) {
	if ((a.min_rank >= b.min_rank) && (a.rank <= b.rank))
		return 1;
	else
		return 0;
}
