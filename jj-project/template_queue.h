/*
 * template_queue.h
 *
 *  Created on: Dec 13, 2016
 *      Author: alex
 */

#ifndef TEMPLATE_QUEUE_H_
#define TEMPLATE_QUEUE_H_

#include <iostream>

template<class T>
class Queue {
	struct QueueNode {
		T data;
		QueueNode *next;
		QueueNode(const T& newData, QueueNode *nextNode) :
				data(newData), next(nextNode) {
		}
	};

	QueueNode *front, *rear;
	int size;
public:
	Queue() :
			front(NULL) ,rear(NULL), size(0) {
		//std::cout << "Queue was constructed" << std::endl;
	}
	~Queue() {
		std::cout << "Queue was destructed" << std::endl;
		while (!isEmpty()) {
			Dequeue();
		}
		isEmpty();
	}

	void Enqueue(const T& object);
	T Dequeue();
	const T& GetfrontData();
	bool isEmpty();
	void Print();
};

template<class T>
void Queue<T>::Enqueue(const T& obj) {
	if (size == 1) {
		front = new QueueNode(obj,rear);
	}
	else {
		QueueNode* tmp = new QueueNode(obj,NULL);
		rear->next = tmp;
		rear = tmp;
	}
	size++;
}

template<class T>
T Queue<T>::Dequeue() {
	if (!isEmpty()) {
		QueueNode *frontNode = front;
		front = front->next;
		T data = frontNode->data;
		delete frontNode;
		size--;
		return data;
	}
	else {
		return 0;
	}
}

template<class T>
const T& Queue<T>::GetfrontData() {
	if (!isEmpty()) {
		return front->data;
	}
}

template<class T>
bool Queue<T>::isEmpty() {
	return (size == 0);
}




#endif /* TEMPLATE_QUEUE_H_ */
