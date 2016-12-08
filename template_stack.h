#ifndef _TEMPLATE__STACK_H
#define _TEMPLATE__STACK_H

#include <iostream>

template<class T>
class Stack {
	struct StackNode {
		T data;
		StackNode *next;
		StackNode(const T& newData, StackNode *nextNode) :
				data(newData), next(nextNode) {
		}
	};

	StackNode *head, *last;
	int size;
public:
	Stack() :
			head(NULL),last(NULL),size(0) {
		//std::cout << "Stack was constructed" << std::endl;
	}
	~Stack() {
		std::cout << "Stack was destructed" << std::endl;
		while (!isEmpty()) {
			Pop();
		}
		isEmpty();
	}

	void Push(const T& object);
	T Pop();
	const T& GetHeadData();
	bool isEmpty();
	void Print();
};

template<class T>
void Stack<T>::Push(const T& obj) {
	head = new StackNode(obj, head);
	size++;
}

template<class T>
T Stack<T>::Pop() {
	if (!isEmpty()) {
		StackNode *headNode = head;
		head = head->next;
		T data = headNode->data;
		delete headNode;
		size--;
		return data;
	}
	else {
		std::cout << "EMPTY STACK" << std::endl;
		return -1;
	}
}

template<class T>
const T& Stack<T>::GetHeadData() {
	if (!isEmpty()) {
		return head->data;
	}
}

template<class T>
bool Stack<T>::isEmpty() {
	return (head == 0);
}

#endif
