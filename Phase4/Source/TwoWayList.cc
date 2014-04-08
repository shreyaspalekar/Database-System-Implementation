#ifndef _TWO_WAY_LIST_C
#define _TWO_WAY_LIST_C

#include "TwoWayList.h"

#include <iostream>
#include <stdlib.h>

using namespace std;


// create an alias of the given TwoWayList
template <class Type>
TwoWayList <Type> :: TwoWayList (TwoWayList &me) {

	list = new (std::nothrow) Header;
	if (list == NULL)
	{
		cout << "ERROR : Not enough memory. EXIT !!!\n";
		exit(1);
	}

	list->first = me.list->first;
	list->last = me.list->last;
	list->current = me.list->current;
	list->leftSize = me.list->leftSize;
	list->rightSize = me.list->rightSize;
}

// basic constructor function
template <class Type>
TwoWayList <Type> :: TwoWayList ()
{

	// allocate space for the header
	list = new (std::nothrow) Header;
	if (list == NULL)
	{
		cout << "ERROR : Not enough memory. EXIT !!!\n";
		exit(1);
	}
	
	// set up the initial values for an empty list
	list->first = new (std::nothrow) Node;
	if (list->first == NULL)
	{
		cout << "ERROR : Not enough memory. EXIT !!!\n";
		exit(1);
	}

	list->last = new (std::nothrow) Node;
	if (list->last == NULL)
	{
		cout << "ERROR : Not enough memory. EXIT !!!\n";
		exit(1);
	}

	list->current = list->first;
	list->leftSize = 0;
	list->rightSize = 0;
	list->first->next = list->last;
	list->last->previous = list->first;

}

// basic deconstructor function
template <class Type>
TwoWayList <Type> :: ~TwoWayList ()
{

	// remove everything
	MoveToStart ();
	while (RightLength ()>0) {
		Type temp;
		Remove (&temp);
	}

	// kill all the nodes
	for (int i = 0; i <= list->leftSize + list->rightSize; i++) {
		list->first = list->first->next;
		delete list->first->previous;
	} 
	delete list->first;
	
	// kill the header
	delete list;

}

// swap operator
template <class Type> void
TwoWayList <Type> :: operator &= (TwoWayList & List)
{

	Header *temp = List.list;
	List.list = list;
	list = temp;
	
}

// make the first node the current node
template <class Type> void
TwoWayList <Type> :: MoveToStart ()
{
	
	list->current = list->first;
	list->rightSize += list->leftSize;
	list->leftSize = 0;

}

// make the first node the current node
template <class Type> void
TwoWayList <Type> :: MoveToFinish ()
{
	
	list->current = list->last->previous;
	list->leftSize += list->rightSize;
	list->rightSize = 0;

}

// determine the number of items to the left of the current node
template <class Type> int 
TwoWayList <Type> :: LeftLength ()
{
	return (list->leftSize);
}

// determine the number of items to the right of the current node
template <class Type> int 
TwoWayList <Type> :: RightLength ()
{
	return (list->rightSize);
}

// swap the right sides of two lists
template <class Type> void 
TwoWayList <Type> :: SwapRights (TwoWayList & List)
{
	
	// swap out everything after the current nodes
	Node *left_1 = list->current;
	Node *right_1 = list->current->next;
	Node *left_2 = List.list->current;
	Node *right_2 = List.list->current->next;

	left_1->next = right_2;
	right_2->previous = left_1;
	left_2->next = right_1;
	right_1->previous = left_2;
	
	// set the new endpoints
	Node *temp = list->last;
	list->last = List.list->last;	
	List.list->last = temp;	

	int tempint = List.list->rightSize;
	List.list->rightSize = list->rightSize;
	list->rightSize = tempint;

}

// swap the leftt sides of the two lists
template <class Type> void 
TwoWayList <Type> :: SwapLefts (TwoWayList & List)
{
	
	// swap out everything after the current nodes
	Node *left_1 = list->current;
	Node *right_1 = list->current->next;
	Node *left_2 = List.list->current;
	Node *right_2 = List.list->current->next;

	left_1->next = right_2;
	right_2->previous = left_1;
	left_2->next = right_1;
	right_1->previous = left_2;

	// set the new frontpoints
	Node *temp = list->first;
	list->first = List.list->first;	
	List.list->first = temp;	

	// set the new current nodes
	temp = list->current;
	list->current = List.list->current;	
	List.list->current = temp;	

	int tempint = List.list->leftSize;
	List.list->leftSize = list->leftSize;
	list->leftSize = tempint;
}

// move forwards through the list 
template <class Type> void 
TwoWayList <Type> :: Advance ()
{

	(list->rightSize)--;
	(list->leftSize)++;
	list->current = list->current->next;

}

// move backwards through the list
template <class Type> void 
TwoWayList <Type> :: Retreat ()
{

	(list->rightSize)++;
	(list->leftSize)--;
	list->current = list->current->previous;

}

// insert an item at the current poition
template <class Type> void
TwoWayList <Type> :: Insert (Type *Item)
{

	Node *temp = new (std::nothrow) Node;
	if (temp == NULL)
	{
		cout << "ERROR : Not enough memory. EXIT !!!\n";
		exit(1);
	}

	Node *left = list->current;
	Node *right = list->current->next;

	left->next = temp;
	temp->previous = left;
	temp->next = right;
	temp->data = new (std::nothrow) Type;
	if (temp->data == NULL)
	{
		cout << "ERROR : Not enough memory. EXIT !!!\n";
		exit(1);
	}

	right->previous = temp;

	temp->data->Consume (Item);

	list->rightSize += 1;

}

// get a reference to the currentitemin the list
template <class Type> Type* 
TwoWayList <Type> ::  Current (int offset)
{
	Node *temp = list->current->next;
	for (int i = 0; i < offset; i++) {
		temp = temp->next;
	}
	return temp->data;
}

// remove an item from the current poition
template <class Type> void
TwoWayList <Type> :: Remove (Type *Item)
{

	Node *temp = list->current->next;
	list->current->next = temp->next;
	temp->next->previous = list->current;
	
	Item->Consume(temp->data);

	delete temp;

	(list->rightSize)--;
}	

#endif
	
