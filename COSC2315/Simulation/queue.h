#include "a:\Boolean.h"
#include <stddef.h>
#include <stdlib.h>
#ifndef QUEUE_POINTER_CLASS_H
#define QUEUE_POINTER_CLASS_H

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
		Q(const Q<T> &Org);
		~Q();

		Boolean Empty() const;
		Boolean EnQ(const T &NewItem);
		Boolean DeQ();
		Boolean DeQ(T &item);
		Boolean GetItem(T &item) const;
	private:
		QNode<T>* BackPtr;

 };

//~~~Implementation Part~~~//

template <class T>
Q<T>::Q():BackPtr(NULL)
 {
 }

template <class T>
Q<T>::Q(const Q<T> &Org)
 {
	BackPtr=NULL;
	QNode<T>* CurrentPtr=Org.BackPtr->Next;
	while (CurrentPtr!=Org.BackPtr)
	 {
		EnQ(CurrentPtr->Item);
		CurrentPtr=CurrentPtr->Next;
	 }
	EnQ(CurrentPtr->Item);
	CurrentPtr=NULL;
 }

template <class T>
Q<T>::~Q()
 {
	while (!Empty())
		DeQ();
 }

template <class T>
Boolean Q<T>::Empty() const
 {
	return Boolean (BackPtr==NULL);
 }

template <class T>
Boolean Q<T>::EnQ(const T& NewItem)
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
Boolean Q<T>::DeQ()
 {
	if (!Empty())
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
Boolean Q<T>::DeQ(T &item)
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
Boolean Q<T>::GetItem(T &item) const
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

#endif