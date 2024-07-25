#pragma once
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <cstddef>

template <typename ValueType> class VectorizedContainer
	{
	private:
		class ListNode
			{
			public:
				intptr_t Previous, Next;
				ValueType Data;
			};

		struct ListStruct
			{
			intptr_t First, Last;
			size_t Count;
			};

		ListNode *Elements;
		ListStruct FreeNodes, UsedNodes;
		size_t Capacity;

		bool IsNodeInList ( const ListStruct &List, const intptr_t Index ) const;
		intptr_t GetNodeFromList ( ListStruct &List );
		void RemoveNodeFromList ( ListStruct &List, const intptr_t Index );
		void AddNodeToList ( ListStruct &List, const intptr_t Index );
		VectorizedContainer <ValueType> &operator = ( const VectorizedContainer<ValueType> &Other ) = delete;
	public:
		bool PackElements;
		class iterator
			{
			protected:
				const VectorizedContainer <ValueType> *Owner;
				intptr_t Index;
			public:
				iterator ( const VectorizedContainer *NewOwner, intptr_t NewIndex );
				iterator ( void );
				iterator ( const iterator &Other );
				iterator &operator ++ ( void );
				iterator &operator -- ( void );
				iterator operator++ ( int );
				iterator operator-- ( int );
				bool operator == ( const iterator &Other ) const;
				bool operator != ( const iterator &Other ) const;
				iterator &operator = ( const iterator &Other );
				explicit operator bool ( void ) const;
				ValueType &operator * ( void ) const;
				ValueType *operator -> ( void ) const;

				size_t GetIndex ( void ) const;
				ValueType &GetValue ( void ) const;
				bool IsValid ( void ) const;
			};

		VectorizedContainer ( void );
		void Reserve ( const size_t NewCapacity );
		size_t GetFreeIndex ( void );
		iterator GetFreeSlot ( void );
		void ReleaseIndex ( const size_t Index );
		void ReleaseSlot ( const iterator Iterator );
		size_t GetSize ( void ) const;
		inline ValueType &operator[] ( const size_t Index ) const;
		iterator begin ( void ) const;
		iterator end ( void ) const;
		iterator first ( void ) const;
		iterator last ( void ) const;
		iterator push ( const ValueType &NewValue );
		iterator iterator_for_index ( const size_t Index ) const;
		bool IsUsed ( const size_t Index );
		void Clear ( void );
		void Debug ( void ( *func ) ( const intptr_t, const ValueType & ) );
		void Debug ( void );
	};

template <typename ValueType>
VectorizedContainer <ValueType>::iterator::iterator ( const VectorizedContainer *NewOwner, intptr_t NewIndex ) : Owner ( NewOwner ), Index ( NewIndex ) {}

template <typename ValueType>
VectorizedContainer <ValueType>::iterator::iterator ( void ) : Owner ( nullptr ), Index ( -1 ) {}

template <typename ValueType>
VectorizedContainer <ValueType>::iterator::iterator ( const iterator &Other ) : Owner ( Other.Owner ), Index ( Other.Index ) {}

template <typename ValueType>
typename VectorizedContainer <ValueType>::iterator &VectorizedContainer <ValueType>::iterator::operator++()
	{
	if ( Index != -1 )
		Index = Owner->Elements[Index].Next;
	return *this;
	}

template <typename ValueType>
typename VectorizedContainer <ValueType>::iterator &VectorizedContainer <ValueType>::iterator::operator--()
	{
	if ( Index != -1 )
		Index = Owner->Elements[Index].Previous;
	return *this;
	}

template <typename ValueType>
typename VectorizedContainer <ValueType>::iterator VectorizedContainer <ValueType>::iterator::operator++ ( int )
	{
	iterator tmp ( *this );
	operator++();
	return tmp;
	}

template <typename ValueType>
typename VectorizedContainer <ValueType>::iterator VectorizedContainer <ValueType>::iterator::operator-- ( int )
	{
	iterator tmp ( *this );
	operator--();
	return tmp;
	}

template <typename ValueType>
bool VectorizedContainer <ValueType>::iterator::operator== ( const iterator &Other ) const
	{
	return Index == Other.Index;
	}

template <typename ValueType>
bool VectorizedContainer <ValueType>::iterator::operator!= ( const iterator &Other ) const
	{
	return Index != Other.Index;
	}

template <typename ValueType>
typename VectorizedContainer <ValueType>::iterator &VectorizedContainer <ValueType>::iterator::operator = ( const iterator &Other )
	{
	Index = Other.Index;
	Owner = Other.Owner;
	return *this;
	}

template <typename ValueType>
VectorizedContainer <ValueType>::iterator::operator bool ( void ) const
	{
	return IsValid();
	}

template <typename ValueType>
ValueType &VectorizedContainer <ValueType>::iterator::operator* ( void ) const
	{
	return GetValue();
	}

template <typename ValueType>
ValueType *VectorizedContainer <ValueType>::iterator::operator-> ( void ) const
	{
	return & ( Owner->Elements[Index].Data );
	}

template <typename ValueType>
size_t VectorizedContainer <ValueType>::iterator::GetIndex ( void ) const
	{
	return Index;
	}

template <typename ValueType>
ValueType &VectorizedContainer <ValueType>::iterator::GetValue ( void ) const
	{
	return Owner->Elements[Index].Data;
	}

template <typename ValueType>
bool VectorizedContainer <ValueType>::iterator::IsValid ( void ) const
	{
	return Index != -1;
	}

template <typename ValueType>
intptr_t VectorizedContainer<ValueType>::GetNodeFromList ( ListStruct &List )
	{
	intptr_t Index = List.First;
	if ( Elements[List.First].Next != -1 ) // If there is a second node, set its Previous value to -1
		Elements[Elements[List.First].Next].Previous = -1;
	List.First = Elements[List.First].Next;
	if ( List.First == -1 ) // If I ran out of nodes
		List.Last = -1;
	Elements[Index].Previous = Elements[Index].Next = -1;
	--List.Count;
	return Index;
	}

template <typename ValueType>
bool VectorizedContainer<ValueType>::IsNodeInList ( const ListStruct &List, const intptr_t Index ) const
	{
	for ( intptr_t IndexIterator = List.First; IndexIterator != -1; IndexIterator = Elements[IndexIterator].Next )
		{
		if ( IndexIterator == Index )
			return true;
		}
	return false;
	}

template <typename ValueType>
void VectorizedContainer<ValueType>::RemoveNodeFromList ( ListStruct &List, const intptr_t Index )
	{
	assert ( List.Count > 0 );
	if ( Elements[Index].Previous != -1 )
		Elements[Elements[Index].Previous].Next = Elements[Index].Next;
	if ( Elements[Index].Next != -1 )
		Elements[Elements[Index].Next].Previous = Elements[Index].Previous;
	if ( List.First == Index )
		List.First = Elements[Index].Next;
	if ( List.Last == Index )
		List.Last = Elements[Index].Previous;
	--List.Count;
	}

template <typename ValueType>
void VectorizedContainer<ValueType>::AddNodeToList ( ListStruct &List, const intptr_t Index )
	{
	if ( List.Count == 0 )
		{
		List.First = List.Last = Index;
		List.Count = 1;
		Elements[Index].Previous = Elements[Index].Next = -1;
		return;
		}

	if ( PackElements )
		{
		// if the new handle comes closer to the head of the buffer than the current First free node, update List.First and be done with it
		if ( Index < List.First )
			{
			Elements[List.First].Previous = Index;
			Elements[Index].Next = List.First;
			Elements[Index].Previous = -1;
			List.First = Index;
			}
		// same thing for the tail of the buffer
		else if ( Index > List.Last )
			{
			Elements[List.Last].Next = Index;
			Elements[Index].Previous = List.Last;
			Elements[Index].Next = -1;
			List.Last = Index;
			}
		else
			{
			// If I want to pack elements as much as possible, the indices need to be ordered, so that the lower-Index free slots are the first to be reused
			intptr_t CurrentFreeNodeIndex = List.First;
			while ( Index > CurrentFreeNodeIndex )
				{
				CurrentFreeNodeIndex = Elements[CurrentFreeNodeIndex].Next;
				assert ( CurrentFreeNodeIndex != -1 );
				}

			// when I get here, it's because I've found an index in the free list which is bigger than my own. insert mine right before it
			Elements[Index].Previous = Elements[CurrentFreeNodeIndex].Previous;
			Elements[Index].Next = CurrentFreeNodeIndex;
			Elements[Elements[CurrentFreeNodeIndex].Previous].Next = Index;
			Elements[CurrentFreeNodeIndex].Previous = Index;
			}
		}
	else
		{
		// Adds this Index to the end of the List
		Elements[List.Last].Next = Index;
		Elements[Index].Previous = List.Last;
		Elements[Index].Next = -1;
		List.Last = Index;
		}
	++List.Count;
	}

template <typename ValueType>
VectorizedContainer<ValueType>::VectorizedContainer ( void )
	{
	FreeNodes.First = FreeNodes.Last = UsedNodes.First = UsedNodes.Last = -1;
	Capacity = FreeNodes.Count = UsedNodes.Count = 0;
	Elements = nullptr;
	PackElements = true;
	}

template <typename ValueType>
void VectorizedContainer<ValueType>::Reserve ( const size_t NewCapacity )
	{
	if ( NewCapacity <= Capacity )
		return;
	size_t delta = NewCapacity - Capacity;
	ListNode *Newpointer = ( ListNode * ) realloc ( ( void * ) Elements, sizeof ( ListNode ) * NewCapacity );
	assert ( Newpointer );
	Elements = Newpointer;

	for ( size_t Index = Capacity; Index < NewCapacity; ++Index ) // Add all the new nodes to the List
		{
		AddNodeToList ( FreeNodes, Index );
		}
	Capacity += delta;
	}

template <typename ValueType>
size_t VectorizedContainer<ValueType>::GetFreeIndex ( void )
	{
	if ( FreeNodes.Count == 0 )
		{
		Reserve ( Capacity + 10 );
		}
	size_t Index = GetNodeFromList ( FreeNodes );
	new ( &Elements[Index] ) ListNode ();
	AddNodeToList ( UsedNodes, Index );
	return Index;
	}

template <typename ValueType>
typename VectorizedContainer<ValueType>::iterator VectorizedContainer<ValueType>::GetFreeSlot ( void )
	{
	size_t Index = GetFreeIndex ();
	return iterator ( this, Index );
	}

template <typename ValueType>
void VectorizedContainer<ValueType>::ReleaseIndex ( const size_t Index )
	{
	assert ( Index < Capacity );

	Elements[Index].~ListNode();
	RemoveNodeFromList ( UsedNodes, Index );
	AddNodeToList ( FreeNodes, Index );
	}

template <typename ValueType>
void VectorizedContainer<ValueType>::ReleaseSlot ( const iterator Iterator )
	{
	if ( !Iterator )
		return;
	ReleaseIndex ( Iterator.Index );
	}

template <typename ValueType>
size_t VectorizedContainer<ValueType>::GetSize ( void ) const
	{
	return Capacity - FreeNodes.Count;
	}

template <typename ValueType>
ValueType &VectorizedContainer<ValueType>::operator[] ( const size_t Index ) const
	{
	assert ( Index < Capacity );
	return Elements[Index].Data;
	}

template <typename ValueType>
typename VectorizedContainer<ValueType>::iterator VectorizedContainer<ValueType>::begin ( void ) const
	{
	if ( UsedNodes.Count == 0 )
		return end();
	return iterator ( this, UsedNodes.First );
	}

template <typename ValueType>
typename VectorizedContainer<ValueType>::iterator VectorizedContainer<ValueType>::end ( void ) const
	{
	return iterator ( this, -1 );
	}

template <typename ValueType>
typename VectorizedContainer<ValueType>::iterator VectorizedContainer<ValueType>::first ( void ) const
	{
	return begin ();
	}

template <typename ValueType>
typename VectorizedContainer<ValueType>::iterator VectorizedContainer<ValueType>::last ( void ) const
	{
	if ( UsedNodes.Count == 0 )
		return end ();
	return iterator ( this, UsedNodes.Last );
	}

template <typename ValueType>
typename VectorizedContainer<ValueType>::iterator VectorizedContainer<ValueType>::push ( const ValueType &NewValue )
	{
	size_t Index = GetFreeIndex ();
	Elements[Index].Data = NewValue;
	return iterator ( this, Index );
	}

template <typename ValueType>
typename VectorizedContainer<ValueType>::iterator VectorizedContainer<ValueType>::iterator_for_index ( const size_t Index ) const
	{
	if ( UsedNodes.Count == 0 )
		return end ();
	return iterator ( this, Index );
	}

template <typename ValueType>
bool VectorizedContainer<ValueType>::IsUsed ( const size_t Index )
	{
	if ( UsedNodes.Count == 0 )
		return false;
	return IsNodeInList ( UsedNodes, Index );
	}

template <typename ValueType>
void VectorizedContainer<ValueType>::Clear ( void )
	{
	if ( Capacity == 0 )
		return;
	intptr_t Index = UsedNodes.First;
	while ( Index != -1 )
		{
		intptr_t Next = Elements[Index].Next;
		Elements[Index].~ListNode();
		Index = Next;
		}
	UsedNodes.First = UsedNodes.Last = -1;
	UsedNodes.Count = 0;
	FreeNodes.First = 0;
	FreeNodes.Last = Capacity - 1;
	FreeNodes.Count = Capacity;
	for ( size_t Index2 = 0; Index2 < Capacity; ++Index2 )
		{
		Elements[Index2].Previous = Index2 - 1;
		Elements[Index2].Next = Index2 + 1;
		}
	Elements[FreeNodes.First].Previous = -1;
	Elements[FreeNodes.Last].Next = -1;
	}

template <typename ValueType>
void VectorizedContainer<ValueType>::Debug ( void ( *func ) ( const intptr_t, const ValueType & ) )
	{
	for ( unsigned Index = 0; Index < Capacity; ++Index )
		{
		func ( Index, Elements[Index] );
		printf ( "%Ii %Ii\n", Elements[Index].Previous, Elements[Index].Next );
		}
	intptr_t Index = FreeNodes.First;
	while ( Index != -1 )
		{
		printf ( "%Ii <- %Iu -> %Ii\n", Elements[Index].Previous, Index, Elements[Index].Next );
		Index = Elements[Index].Next;
		}
	}

template <typename ValueType>
void VectorizedContainer<ValueType>::Debug ( void )
	{
	printf ( "Complete dump\n" );
	for ( unsigned Index = 0; Index < Capacity; ++Index )
		{
		printf ( "%Ii %u %Ii\n", Elements[Index].Previous, Index, Elements[Index].Next );
		}
	printf ( "Free node list\n" );
	intptr_t Index = FreeNodes.First;
	while ( Index != -1 )
		{
		printf ( "%Ii <- %Iu -> %Ii\n", Elements[Index].Previous, Index, Elements[Index].Next );
		Index = Elements[Index].Next;
		}
	}
