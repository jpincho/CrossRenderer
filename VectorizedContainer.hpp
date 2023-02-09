#pragma once

template <typename Type> class VectorizedContainer
    {
    private:
        typedef struct ListNode : public Type
            {
            intptr_t Previous, Next;
            } ListNode;

        ListNode *Objects;
        size_t FreeCount, TotalCount;
        size_t FirstFree, LastFree;

    public:
        VectorizedContainer ( void )
            {
            FirstFree = LastFree = 0;
            FreeCount = TotalCount = 0;
            Objects = nullptr;
            }
        void Reserve ( const size_t NewSize )
            {
            if ( NewSize <= TotalCount )
                return;
            size_t Delta = NewSize - TotalCount;
			void *TempPointer = realloc ( Objects, sizeof ( ListNode ) * NewSize );
			assert ( TempPointer );
			Objects = (ListNode *) TempPointer;
            assert ( Objects );
            if ( TotalCount == 0 ) // First allocation. Initialize the first node.
                {
                FirstFree = LastFree = 0;
                Objects[FirstFree].Previous = Objects[FirstFree].Next = -1;
                new ( &Objects[FirstFree] ) ListNode ();
                }
            for ( unsigned Index = 0; Index < Delta - 1; ++Index ) // Add all the new nodes to the list
                {
                new ( Objects + LastFree + 1 ) ListNode ();
                Objects[LastFree].Next = LastFree + 1;
                Objects[LastFree + 1].Previous = LastFree;
                ++LastFree;
                }
            Objects[LastFree].Next = -1;
            TotalCount = NewSize;
            FreeCount += Delta;
            }
        size_t GetFreeIndex ( void )
            {
            if ( FreeCount == 0 )
                {
                Reserve ( TotalCount + 10 );
                }
            // Get the first free index from the list, saves it for return, and updates FirstFree to the next index if there is one
            size_t FreeIndex = FirstFree;
            if ( Objects[FirstFree].Next != -1 )
                Objects[Objects[FirstFree].Next].Previous = -1;
            FirstFree = Objects[FirstFree].Next;
            --FreeCount;
            Objects[FreeIndex].Previous = Objects[FreeIndex].Next = -1;
            return FreeIndex;
            }
        void ReleaseIndex ( const size_t Index )
            {
            assert ( Index < TotalCount );

            Objects[Index].~ListNode ();

            // The simplified case where there are no more free nodes
            if ( FreeCount == 0 )
                {
                FirstFree = LastFree = Index;
                Objects[Index].Previous = Objects[Index].Next = -1;
                ++FreeCount;
                return;
                }
#if defined ( VECTORIZED_CONTAINER_PACK_OBJECTS )
            // if the new handle comes closer to the head of the buffer than the current first free node, update FirstFree and be done with it
            if ( Index < FirstFree )
                {
                Objects[FirstFree].Previous = Index;
                Objects[Index].Next = FirstFree;
                Objects[Index].Previous = -1;
                FirstFree = Index;
                }
            // same thing for the tail of the buffer
            else if ( Index > LastFree )
                {
                Objects[LastFree].Next = Index;
                Objects[Index].Previous = LastFree;
                Objects[Index].Next = -1;
                LastFree = Index;
                }
            else
                {
                // If I want to pack objects as much as possible, the indices need to be ordered, so that the lower-index free slots are the first to be reused
                size_t CurrentFreeIndex = FirstFree;
                while ( Objects[CurrentFreeIndex].Next != -1 )
                    {
                    if ( Index > CurrentFreeIndex )
                        {
                        CurrentFreeIndex = Objects[CurrentFreeIndex].Next;
                        continue;
                        }
                    // when I get here, it's because I've found an index in the free list which is bigger than my own. insert mine right before it
                    Objects[Index].Previous = Objects[CurrentFreeIndex].Previous;
                    Objects[Index].Next = CurrentFreeIndex;
                    Objects[Objects[CurrentFreeIndex].Previous].Next = Index;
                    Objects[CurrentFreeIndex].Previous = Index;
                    break;
                    }
                }
#else
            // Adds this index to the end of the list
            Objects[LastFree].Next = Index;
            Objects[Index].Previous = LastFree;
            Objects[Index].Next = -1;
            LastFree = Index;
#endif
            ++FreeCount;
            }

        Type &operator [] ( const size_t Index )
            {
            assert ( Index < TotalCount );
            return Objects[Index];
            }
    };
