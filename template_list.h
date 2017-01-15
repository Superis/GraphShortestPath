/*
 * template_list.h
 *
 *  Created on: Dec 8, 2016
 *      Author: alex
 */

#ifndef TEMPLATE_LIST_H_
#define TEMPLATE_LIST_H_
#include <iostream>

template<class T>
class List {
	struct ListNode {
		T data;
		ListNode *next,*prev;
		ListNode(const T& newData) :
				data(newData), next(NULL), prev(NULL) {
		}
	};
	ListNode *head,*cur,*last;
	int size;
public:
	List() :
			head(NULL),cur(NULL), last(NULL), size(0) {
		//std::cout << "List was constructed" << std::endl;
	}
	~List() {
		std::cout << "List is destructed" << std::endl;
		while (!isEmpty()) {
			PopLast();
		}
		isEmpty();
	}

	void Push(const T& object);
	T PopHead();
	T PopLast();
	bool IsOut();
	const T& GetCurData();
	const T& GetHeadData();
	int GetUnvisitedEdge(int**);
	bool isEmpty();
	void ResetCur() { cur = head; };
	bool IncCur();
	void Print();
};

template<class T>
void List<T>::Push(const T& obj) {
	if (size == 0) {
		head = new ListNode(obj);
		last = head;
	}
	else {
		ListNode* tmp = new ListNode(obj);
		tmp->prev = last;
		last->next = tmp;
		last = tmp;
	}
	size++;
}

template<class T>
T List<T>::PopHead() {
	if (!isEmpty()) {
		T data;
		if (size == 1) {
			head->next = NULL;
			head->prev = NULL;
			last->prev = NULL;
			last->next = NULL;
			last = NULL;
			data = head->data;
			delete head;
			head = NULL;
		} else if (size > 1) {
			head = head->next;
			data = head->prev->data;
			delete head->prev;
			head->prev = NULL;
		}
		size--;
		return data;
	} else {
		std::cout << "\t Pophead : EMPTY LIST" << std::endl;
		return 0;
	}
}

template<class T>
T List<T>::PopLast() {
	if (!isEmpty()) {
		T data;
		if (size == 1) {
			head->next = NULL;
			head->prev = NULL;
			last->prev = NULL;
			last->next = NULL;
			last = NULL;
			data = head->data;
			delete head;
			head = NULL;
		} else if (size > 1) {
			last = last->prev;
			data = last->next->data;
			delete last->next;
			last->next = NULL;
		}
		size--;
		return data;
	} else {
		std::cout << "\tPopLast : EMPTY LIST" << std::endl;
		return 0;
	}
}

template<class T>
const T& List<T>::GetHeadData() {
	if (!isEmpty()) {
		return head->data;
	}
}

template<class T>
bool List<T>::IsOut() {
	if (!isEmpty()) {
		return (cur == NULL);
	}
	std::cout << "Return false LIST::ISOUT" << std::endl;
	return false;
}

template<class T>
bool List<T>::isEmpty() {
	return (head == 0);
}

template<class T>
const T& List<T>::GetCurData() {
	return cur->data;
}

template<class T>
bool List<T>::IncCur() {
	if ( (cur->next) != NULL ) {
		cur = cur->next;
		return true;
	} else
		return false;
}

template<class T>
int  List<T>::GetUnvisitedEdge(int** visited) {
	ListNode* p= head;
	while (p!=NULL){
		if ((*(visited[p->data]))==0)
			return p->data;
		p=p->next;
	}
	return -1;
}

template<class T>
void List<T>::Print() {
	std::cout << "Printing List : " << std::endl;
	ListNode *cur = head;
	for (int i = size; i > 1; i--) {
		std::cout << cur->data << " ";
		cur = cur->next;
	}
	std::cout <<cur->data << std::endl;
}

#endif
