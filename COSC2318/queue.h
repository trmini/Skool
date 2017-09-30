#include "a:\Boolean.h"
#ifndef QUEUE_CLASS_H
#define QUEUE_CLASS_H

template <class T>
 struct QNode
  {
	T Item;
	QNode<T> *Next;
  };

template <class T>
class Q
 {
	public:
		Q();
		Q(const Q Org);
		~Q;

		Boolean Empty() const;
		Boolean EnQ(const T &NewItem);
		Boolean DeQ();
		Boolean EnQ(T &item);
		Boolean GetItem(T &item);
	private:
		QNode<T>* BackPtr;

 };

//~~~Implementation Part~~~//

template <class T>
Q::Q():BackPtr(NULL)
 {
 }

template <class T>
Q::Q(const Q &Org)
 {

 }

template <class T>
Q::~Q()
 {
	while (!Empty())
		DeQ();
 }

template <class T>
Boolean Q::Empty() const
 {
	return Boolean (BackPtr==NULL);
 }

template <class T>
Boolean Q::EnQ(const T& NewItem)
 {
	QNode<T> *NewPtr= new QNode<T>;
	if (NewPtr!=NULL)
	 {
		NewPtr->Item=NewItem;
		if (Empty())
		 {
			NewPtr->Next=NewPtr;
		 }
		else
		 {
			NewPtr->Next=BackPtr->Next;
			BackPtr->Next=NewPtr;
		 }
		BackPtr=NewPtr;
		return True;
	 }
	else
	 return False;
 }

template <class T>
Boolean Q::DeQ()
 {
	if (!Empty)
	 {
		QNode<T> *FrontPtr=BackPtr->Next;
		if (FrontPtr==BackPtr)
			BackPtr=NULL;
		else
			BackPtr->Next=FrontPtr->Next;
		FrontPtr->Next=NULL;
		delete FrontPtr;
		return True;
	 }
	else
	 return False;
 }

template <class T>
Boolean Q::DeQ(T &item)
 {
	if (!Empty())
	 {
		QNode<T> *FrontPtr=BackPtr->Next;
		if (FrontPtr==BackPtr)
			BackPtr=NULL;
		else
			BackPtr->Next=FrontPtr->Next;
		FrontPtr->Next=NULL;
		item=FrontPtr->Item;
		delete FrontPtr;
		return True;
	 }
	else
		return False;
 }

template <class T>
Boolean Q::GetItem(T &item) const
 {
	if (!Empty())
	 {
		QNode<T> *FrontPtr=BackPtr->Next;
		item=FrontPtr->Item;
		return True;
	 }
	else
	 return False;
 }