#ifndef ADDRESS_BOOK_TREE_H
#define ADDRESS_BOOK_TREE_H

#include "c:\mydocu~1\cosc2315\address\people.h"
#include "c:\mydocu~1\cosc2315\address\function.h"
#include <stdlib.h>
#include <string.h>
#include <fstream.h>

typedef char* keyType;
typedef People itemType;

typedef void (*functionType) (itemType& AnItem);

struct TreeNode
 {
	itemType Item;
	TreeNode* LPtr;
	TreeNode* RPtr;

	TreeNode();
 };

TreeNode::TreeNode()
 {
	LPtr=NULL;
	RPtr=NULL;
 }

typedef TreeNode* ptrType;

class PeopleTree
 {
	public:
		PeopleTree();
		PeopleTree(const PeopleTree& Tree);
		virtual ~PeopleTree();

		virtual Boolean Destroy();
		virtual Boolean Empty() const;
		virtual Boolean Insert(const itemType& newItem);
		virtual Boolean Delete(keyType searchKey);
		virtual Boolean Get(keyType searchKey,itemType& treeItem) const;
		virtual void PreOrderTraverse(functionType Visit);
		virtual void InOrderTraverse(functionType Visit);
		virtual void PostOrderTraverse(functionType Visit);

		virtual void Save(ofstream& outfile);
		//Save tree in PreOrder

//		virtual PeopleTree& operator=(const PeopleTree& Rhs);

	protected:
		Boolean InsertItem(ptrType& TreePtr,const itemType& newItem);
		Boolean DeleteItem(ptrType& TreePtr,keyType searchKey);

		void DelRootItem(ptrType& RootPtr);
		void GoLeftMost(ptrType& NodePtr,itemType& treeItem);

		Boolean GetItem(ptrType TreePtr,keyType searchKey,itemType& treeItem) const;

		void CopyTree(ptrType TreePtr, ptrType &NewTreePtr);
		void DestroyTree(ptrType &TreePtr);

		void PreOrder(ptrType TreePtr, functionType Visit);
		void InOrder(ptrType TreePtr, functionType Visit);
		void PostOrder(ptrType TreePtr, functionType Visit);

		ptrType RootPtr() const;
		void SetRootPtr(ptrType newRoot);
		void GetChildPtrs(ptrType NodePtr,ptrType &LChildPtr,ptrType &RChildPtr);
		void SetChildPtrs(ptrType NodePtr,ptrType LChildPtr,ptrType RChildPtr);

		void SaveTree(ptrType& TreePtr,ofstream& tofile);
	private:
		ptrType Root;

 };

PeopleTree::PeopleTree()
 {
	Root=NULL;
 }

PeopleTree::PeopleTree(const PeopleTree& Tree)
 {
	CopyTree(Tree.Root,Root);
 }

PeopleTree::~PeopleTree()
 {
	DestroyTree(Root);
 }

Boolean PeopleTree::Destroy()
 {
	DestroyTree(Root);
	return True;
 }

Boolean PeopleTree::Empty() const
 {
	return Boolean (Root==NULL);
 }

Boolean PeopleTree::Insert(const itemType& newItem)
 {
	return InsertItem(Root,newItem);
 }

Boolean PeopleTree::Delete(keyType searchKey)
 {
	return DeleteItem(Root,searchKey);
 }

Boolean PeopleTree::Get(keyType searchKey,itemType& treeItem) const
 {
	return GetItem(Root,searchKey,treeItem);
 }

void PeopleTree::PreOrderTraverse(functionType Visit)
 {
	PreOrder(Root,Visit);
 }

void PeopleTree::InOrderTraverse(functionType Visit)
 {
	InOrder(Root,Visit);
 }

void PeopleTree::PostOrderTraverse(functionType Visit)
 {
	PostOrder(Root,Visit);
 }

void PeopleTree::Save(ofstream& outfile)
 {
	SaveTree(Root,outfile);
 }

Boolean PeopleTree::InsertItem(ptrType& TreePtr,const itemType& newItem)
 {
	if (TreePtr==NULL)
	 {
		TreePtr=new TreeNode;
		TreePtr->Item=newItem;
		TreePtr->LPtr=NULL;
		TreePtr->RPtr=NULL;
		return Boolean(TreePtr!=NULL);
	 }
	else
	 {
		if (newItem<TreePtr->Item)
			return InsertItem(TreePtr->LPtr,newItem);
		else
			return InsertItem(TreePtr->RPtr,newItem);
	 }
 }

Boolean PeopleTree::DeleteItem(ptrType& TreePtr,keyType searchKey)
 {
	if (TreePtr==NULL)
		return False;
	else
	 {
		if (compare(searchKey,TreePtr->Item.Key())==0)//use strcompare....
		 {
			DelRootItem(TreePtr);
			return True;
		 }
		else
		 {
			if (compare(searchKey,TreePtr->Item.Key())<0)
				return DeleteItem(TreePtr->LPtr,searchKey);
			else
				return DeleteItem(TreePtr->RPtr,searchKey);
		 }
	 }
 }

void PeopleTree::DelRootItem(ptrType& RootPtr)
 {
	ptrType DelPtr;
	itemType ReplacementItem;

	if (RootPtr->LPtr==NULL&&RootPtr->RPtr==NULL)
	 {
		delete RootPtr;
		RootPtr=NULL;
		return;
	 }
	else
	 {
		if (RootPtr->LPtr==NULL)
		 {
			DelPtr=RootPtr;
			RootPtr=RootPtr->RPtr;
			DelPtr->RPtr=NULL;
			delete DelPtr;
			return;
		 }
		else
		 {
			if (RootPtr->RPtr==NULL)
			 {
				DelPtr=RootPtr;
				RootPtr=RootPtr->LPtr;
				DelPtr->LPtr=NULL;
				delete DelPtr;
				return;
			 }
			else
			 {
				GoLeftMost(RootPtr->RPtr,ReplacementItem);
				RootPtr->Item=ReplacementItem;
			 }
		 }
	 }
 }

void PeopleTree::GoLeftMost(ptrType& NodePtr,itemType& treeItem)
 {
	if (NodePtr->LPtr==NULL)
	 {
		treeItem=NodePtr->Item;
		ptrType DelPtr=NodePtr;
		NodePtr=NodePtr->RPtr;
		DelPtr->RPtr=NULL;
		delete DelPtr;
		return;
	 }
	else
	 {
		GoLeftMost(NodePtr->LPtr,treeItem);
	 }
 }

Boolean PeopleTree::GetItem(ptrType TreePtr,keyType searchKey,itemType& treeItem) const
 {
	if (TreePtr==NULL)
	 return False;
	else
	 {
		if (compare(searchKey,TreePtr->Item.Key())==0)
			{
				treeItem=TreePtr->Item;
				return True;
			}
		else
		 {
			if (compare(searchKey,TreePtr->Item.Key())<0)
				return GetItem(TreePtr->LPtr,searchKey,treeItem);
			else
				return GetItem(TreePtr->RPtr,searchKey,treeItem);
		 }
	 }
 }

void PeopleTree::CopyTree(ptrType TreePtr, ptrType &NewTreePtr)
 {
	if (TreePtr!=NULL)
	 {
		NewTreePtr=new TreeNode;
		NewTreePtr->Item=TreePtr->Item;
		NewTreePtr->LPtr=NULL;
		NewTreePtr->RPtr=NULL;
		assert(NewTreePtr!=NULL);

		CopyTree(TreePtr->LPtr,NewTreePtr->LPtr);
		CopyTree(TreePtr->RPtr,NewTreePtr->RPtr);
	 }
	else
		NewTreePtr=NULL;
 }

void PeopleTree::DestroyTree(ptrType &TreePtr)
 {
	if (TreePtr!=NULL)
	 {
		DestroyTree(TreePtr->LPtr);
		DestroyTree(TreePtr->RPtr);
		delete TreePtr;
		TreePtr=NULL;
	 }
 }

void PeopleTree::PreOrder(ptrType TreePtr, functionType Visit)
 {
	if (TreePtr!=NULL)
	 {
		Visit(TreePtr->Item);
		PreOrder(TreePtr->LPtr,Visit);
		PreOrder(TreePtr->RPtr,Visit);
	 }
 }

void PeopleTree::InOrder(ptrType TreePtr, functionType Visit)
 {
	if (TreePtr!=NULL)
	 {
		InOrder(TreePtr->LPtr,Visit);
		Visit(TreePtr->Item);
		InOrder(TreePtr->RPtr,Visit);
	 }
 }

void PeopleTree::PostOrder(ptrType TreePtr, functionType Visit)
 {
	if (TreePtr!=NULL)
	 {
		PostOrder(TreePtr->LPtr,Visit);
		PostOrder(TreePtr->RPtr,Visit);
		Visit(TreePtr->Item);
	 }
 }

ptrType PeopleTree::RootPtr() const
 {
	return Root;
 }
void PeopleTree::SetRootPtr(ptrType newRoot)
 {
	Root=newRoot;
 }

void PeopleTree::GetChildPtrs(ptrType NodePtr,ptrType &LChildPtr,ptrType &RChildPtr)
 {
	LChildPtr=NodePtr->LPtr;
	RChildPtr=NodePtr->RPtr;
 }

void PeopleTree::SetChildPtrs(ptrType NodePtr,ptrType LChildPtr,ptrType RChildPtr)
 {
	NodePtr->LPtr=LChildPtr;
	NodePtr->RPtr=RChildPtr;
 }

void PeopleTree::SaveTree(ptrType& TreePtr,ofstream& tofile)
 {
	if (TreePtr!=NULL)
	 {
		SaveRecord(tofile,TreePtr->Item);
		SaveTree(TreePtr->LPtr,tofile);
		SaveTree(TreePtr->RPtr,tofile);
	 }
 }


#endif