/*
 * components.cpp
 *
 *  Created on: Dec 1, 2016
 *      Author: alex
 */

#include "components.h"

using namespace std;

SCC::SCC() : componentsCount(0), level(0) {
	//components = new List();
}

SCC::~SCC() {
	//delete components;
}

SCC* SCC::EstimateSCC(Buffer* buffer, Index* index) {
	IndexNode *indArr = index->GetIndexNode();
	int indexSize = index->GetSize();
	Stack<int>* stack = new Stack<int>();
	for (int i = 1 ; i < indexSize ; i++) {
		// if Node is undefined : Tarjan
		if (indArr[i].index == -1) {
			Tarjan(i,stack,index,buffer);
		}
	}
	delete stack;
	return NULL;
}

void SCC::Tarjan(int target,Stack<int>* stack,Index* index,Buffer* buffer) {
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
	while(level <= size) {
		// DFS
		neighborSum = index->NeighboursNum(target,'o',buffer);
		curlevel = indArr[target].recursive_level;
		if ( neighborSum > curlevel ) {
			child = out[indArr[target].out].GetNeighbor(indArr[target].recursive_level);
			if (child < 0) {
				cout << "Tarjan found child with subzero value" << endl;
			}
			indArr[target].recursive_level++;
			if (indArr[child].visited == false) {
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
				this->componentsCount++;
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
				components.Push(comp);
			}
			// "recurse" one level backwards
			int newTarget = indArr[target].parentNode;
			if (newTarget != -10) {
				indArr[newTarget].lowlink = MIN(indArr[newTarget].index,indArr[target].lowlink);
				target = newTarget;
			} else // if root ,break while loop.Finished
				break;
		}
	}

}

void SCC::Print() {
	cout << "Found " << componentsCount << " overall SCC : " << level << endl;
	Component *temp;
	while(!components.isEmpty()) {
		temp = components.PopHead();
		cout << "Printing component #ID : " << temp->componentID << endl;
		temp->includedNodesID.Print();
	}
}

