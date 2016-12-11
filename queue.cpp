#include <iostream>
#include "queue.h"

using namespace std;

queue::queue(int tnode):node(tnode)
{
	next = NULL;
}

queue::~queue()
{}

void queue::change_next(int tnode)
{
	next = new queue(tnode);
}

void queue::enqueue(int tnode)
{
	queue* temp;

	temp = this;
	while(temp->next != NULL)
	{
		temp = temp->next;
	}

	temp->change_next(tnode);	
}



int queue::dequeue()
{
	queue* temp;

	temp = this;
	temp = temp->next;

	int frontNode = temp->node;
	 
	return frontNode;
}

queue* queue::get_next()
{
	return next;
}

int queue::front(){
	if(head!=NULL){
	
	int front_elem=(*head)->get_node();
	return front_elem;
	}
	return -1;
}

int queue::get_node()
{
	return node;
}

void queue::print()
{cout << "PRINT" << endl;
	queue* temp;
	temp = this;

	while(temp != NULL)
	{
		cout << temp->node << endl;
		temp = temp->next;
	}
cout << "END" << endl;
}

void enqueue(queue** head, int tsock)
{
		if(*head == NULL)
		*head = new queue(tsock);
		else
		(*head)->enqueue(tsock);
}

int dequeue(queue** head)
{
	if(head!=NULL){
		int x = (*head)->get_node();
		queue* temp = *head;
		(*head) = (*head)->get_next();
		delete temp;
		//cout << "DEQUEUE" << endl;
		return x;
	} 
	cout<<"queue is empty"<<endl;
	return -1;
}

int empty(queue* head)
{
	if(head == NULL) return 1;
	else return 0;
}