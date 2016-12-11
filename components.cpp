/*
 * components.cpp
 *
 *  Created on: Dec 1, 2016
 *      Author: alex
 */

#include "components.h"

using namespace std;

SCC::SCC() : componentsCount(0), level(0) {
	components = new Component*[_size];
}

SCC::~SCC() {
	//delete components;
}

SCC* SCC::EstimateSCC(Buffer* buffer, Index* index ,int max) {
	IndexNode *indArr = index->GetIndexNode();
	int indexSize = index->GetSize();
	Stack<int>* stack = new Stack<int>();
	for (int i = 0 ; i < indexSize ; i++) {
		// if Node is undefined : Tarjan
		if (indArr[i].index == -1) {
			Tarjan(i,stack,index,buffer,max);
		}
	}
	delete stack;
	return NULL;
}


void SCC::Tarjan(int target,Stack<int>* stack,Index* index,Buffer* buffer,int max) {
	cout << "Entered node " << target << " tarjan" << endl;
	IndexNode *indArr = index->GetIndexNode();
	indArr[target].index = level;
	indArr[target].lowlink = level;
	level++;
	stack->Push(target);
	indArr[target].visited = true;
	indArr[target].parentNode = -10; // root value
	indArr[target].recursive_level = 0;
	Node *out = buffer->GetListNode('o');
	if (out == NULL) {
		cout << "\tERROR!Tarjan failed" << endl;
		return;
	}
	int size = index->GetSize() ;
	int neighborSum = 0 ,curlevel = 0 ,child = -1;
	while(1) {
		// DFS
		neighborSum = index->NeighboursNum(target,'o',buffer);
		curlevel = indArr[target].recursive_level;
		if ( neighborSum > curlevel ) {
			//cout << curlevel << " @ " << target << " & " << index->NeighboursNum(target,'o',buffer) << endl;
			child = index->GetNeighbor(target,buffer);//out[indArr[target].out].GetNeighbor(indArr[target].recursive_level,target,index,buffer);
			if (child < 0 || child > max) {
				cout << "Tarjan found child with unidentified value :" << child << endl;
				break;
			}
			indArr[target].recursive_level++;
			if (indArr[child].index == -1) {
				indArr[child].index = level;
				indArr[child].lowlink = level;
				indArr[child].parentNode = target;
				indArr[child].recursive_level = 0;
				level++;
				stack->Push(child);
				indArr[child].visited = true;
				target = child;
			} else if (indArr[child].visited == true) {
				indArr[target].lowlink = MIN(indArr[child].lowlink,indArr[target].lowlink);
			}
		} else {
			// found a SCC
			if (indArr[target].lowlink == indArr[target].index) {
				Component* comp = new Component();
				comp->componentID = this->componentsCount;
				int top = stack->Pop();
				if (top == -1) {
					cout << "top(1) pop empty" << endl;
					break;
				}
				indArr[top].visited = false;
				comp->includedNodesID.Push(top);
				int size = 1;
				while (top != target) {
					top = stack->Pop();
					if (top == -1) {
						cout << "top(2) pop empty" << endl;
						break;
					}
					comp->includedNodesID.Push(top);
					indArr[top].visited = false;
					size++;
				}
				comp->nodesSum = size;
				this->AddComponentToArray(comp);
				target = indArr[target].parentNode;
				if (target == -10)
					break;
				else
					continue;
			} else {
				// "recurse" one level backwards
				int newTarget = indArr[target].parentNode;
				if (newTarget != -10) {
					indArr[newTarget].lowlink = MIN(indArr[newTarget].index,
							indArr[target].lowlink);
					target = newTarget;
				} else
					// if root ,break while loop.Finished
				break;
			}
		}
	}
}

void SCC::AddComponentToArray(Component* comp) {
	if (componentsCount < size) {
		components[componentsCount] = comp;
		componentsCount++;
	} else {
		Component** newcomp = new Component*[2*size];
		for (int i = 0 ; i < size ; i++) {
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

	}
}

void SCC::Print() {
	cout << "Found " << componentsCount << " overall SCC : " << level << endl;
	for (int i = 0 ; i < componentsCount ; i++) {
		components[i]->includedNodesID.Print();
	}
	//Component *temp;
	//while(!components.isEmpty()) {
		//temp = components.PopHead();
		//cout << "Printing component #ID : " << temp->componentID << endl;
		//temp->includedNodesID.Print();
	//}
}



int SCC::EstimateShortestPathSCC(Buffer* buffer,Index* index,int compsrc_arg,int compdest_arg,int src ,int dest){
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


}

















void  SCC::BuildHypergraph(Index* index,Buffer* buffer){
		int node_pos,int current_component;
		index_node* k=index->GetIndexNode();
			Node* G=buffer->GetListNode('o');
		for (int i=0;i<componentsCount;i++){
			current_component=this->components[i]->component_id;
			int current_node=k[components[i]->includedNodesID.GetCurData()].out
			while (current_node != NULL){
				node_pos=k[components[i]->includedNodesID[j]].out;
				if (node_pos!=-1)
				do{
					node_pos=G[node_pos].SearchDiffComponent(current_component);
				}while(node_pos);
			}
		}
}



void SCC::BuildGrailIndex(){
	srand(time(NULL));
	int visit_number=0;
	int visited[componentsCount]={-1};
	r=1;
	int i;
	int k=rand() % componentsCount;
	while (visit_number < componentsCount){
		for (i =0;i<componentsCount;i++;)
			if (visited[i]==0)
				break;
		this->GrailProgress(i,&visited,&r,&num);
	}
}



void SCC::GrailProgress(int i,int* visited[],int* r,int* num){
	int min_rank=2; //timi wste me tin prwti na paroume mikrotero min_rank
	int flag[componentsCount]={0};
	int new_progress;
	Stack<int> StackProgress;
	StackProgress.Push(i);

	//i=StackProgress.Pop();
	while (!StackProgress.isEmpty()){
		//i=StackProgress.Pop();
		if (visited[i]){
			min_rank=min(min_rank,components[i]->label.min_rank);
			StackProgress.Pop();
			continue;
		}
		while ((new_progress=this->edges[i]->GetUnvisitedEdge(visited))!=-1){
			flag[i]=1;
			i=new_progress;
			StackProgress.Push(i);
		}
		components[i].label.rank=r;
		if (!flag){
			components[i].label.min_rank=r;
			min_rank=min(min_rank,r);
		}
		else
			components[i].label.min_rank=min_rank;
		visited[i]=1;
		num*++;
		r*++;
	  StackProgress.Pop();
	}

}

GRAIL_ANSWER SCC::IsReachableGrail(Index* index,int source,int dest){
	index_node* indArray=index->GetIndexNode();
	if (indArray[source].componentID == indArray[dest].componentID)
		return YES;
	else{  // ELEGXOUME TO EURETIRIO GRAIL
		if (this->Subset(components[indArray[dest].componentID].label,components[indArray[source].componentID].label))
			return MAYBE;
		else
			return NO;
	}
}



int SCC::Subset(Label a,Label b){
	if ((a.min_rank >= b.min_rank) && (a.rank <= b.rank))
		return 1;
	else
		return 0;
}
