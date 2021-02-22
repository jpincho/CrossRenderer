#pragma once
#include <assert.h>

namespace CrossRenderer
{
template <typename TypeTag>
class HandleTemplate
    {
    private:
        intptr_t Key;
    public:
        HandleTemplate ( void )
            {
            Key = -1;
            }
        explicit HandleTemplate ( intptr_t NewKey )
            {
            Key = NewKey;
            }
        intptr_t key ( void ) const
            {
            return Key;
            }
        bool operator == ( const HandleTemplate <TypeTag> &Other ) const
            {
            return ( Key == Other.Key );
            }
        bool operator != ( const HandleTemplate <TypeTag> &Other ) const
            {
            return ( Key != Other.Key );
            }
        bool operator < ( const HandleTemplate <TypeTag> &Other ) const
            {
            return ( Key < Other.Key );
            }
        operator bool ( void ) const
            {
            return *this != invalid;
            }
        static const HandleTemplate<TypeTag> invalid;
    };

template <typename TypeTag>
const HandleTemplate<TypeTag> HandleTemplate<TypeTag>::invalid;


template <typename Type, typename TypeTag> class VectorizedContainer
    {
    private:
        typedef struct ListNode : public Type
            {
            intptr_t Previous, Next;
            } ListNode;

        ListNode *Objects;
        size_t FreeCount, TotalCount;
        intptr_t FirstFree, LastFree;

    public:
        VectorizedContainer ( void )
            {
            FirstFree = -1;
            LastFree = -1;
            FreeCount = TotalCount = 0;
            Objects = nullptr;
            }
        void Reserve ( const size_t NewSize )
            {
            if ( NewSize <= TotalCount )
                return;
            if ( NewSize == 0 )
                return;
            size_t Delta = NewSize - TotalCount;
            Objects = ( ListNode * ) realloc ( Objects, sizeof ( ListNode ) * NewSize );
            TotalCount = NewSize;
            if ( FirstFree == -1 ) // First allocation. Initialize the first node.
                {
                FirstFree = LastFree = 0;
                Objects[FirstFree].Previous = Objects[FirstFree].Next = -1;
                new ( Objects ) ListNode ();
                }
            for ( unsigned Index = 0; Index < Delta - 1; ++Index ) // Add all the new nodes to the list
                {
                new ( Objects + LastFree + 1 ) ListNode ();
                Objects[LastFree].Next = LastFree + 1;
                Objects[LastFree + 1].Previous = LastFree;
                ++LastFree;
                }
            Objects[LastFree].Next = -1;
            FreeCount += Delta;
            }
        HandleTemplate<TypeTag> GetNewHandle ( void )
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
            return HandleTemplate <TypeTag> ( FreeIndex );
            }
        void ReleaseHandle ( const HandleTemplate<TypeTag> &Handle )
            {
            assert ( Handle );
            assert ( ( unsigned long ) Handle.key() < TotalCount );

            Objects[Handle.key ()].~ListNode ();

            // The simplified case where there are no more free nodes
            if ( FreeCount == 0 )
                {
                FirstFree = LastFree = Handle.key ();
                Objects[Handle.key ()].Previous = Objects[Handle.key ()].Next = -1;
                ++FreeCount;
                return;
                }
#define VECTORIZED_CONTAINER_PACK_OBJECTS
#if defined ( VECTORIZED_CONTAINER_PACK_OBJECTS )
            // if the new handle comes closer to the head of the buffer than the current first free node, update FirstFree and be done with it
            if ( Handle.key () < FirstFree )
                {
                Objects[FirstFree].Previous = Handle.key ();
                Objects[Handle.key ()].Next = FirstFree;
                Objects[Handle.key ()].Previous = -1;
                FirstFree = Handle.key ();
                }
            // same thing for the tail of the buffer
            else if ( Handle.key () > LastFree )
                {
                Objects[LastFree].Next = Handle.key ();
                Objects[Handle.key ()].Previous = LastFree;
                Objects[Handle.key ()].Next = -1;
                LastFree = Handle.key ();
                }
            else
                {
                // If I want to pack objects as much as possible, the indices need to be ordered, so that the lower-index free slots are the first to be reused
                intptr_t CurrentFreeIndex = FirstFree;
                while ( Objects[CurrentFreeIndex].Next != -1 )
                    {
                    if ( Handle.key () > CurrentFreeIndex )
                        {
                        CurrentFreeIndex = Objects[CurrentFreeIndex].Next;
                        continue;
                        }
                    // when I get here, it's because I've found an index in the free list which is bigger than my own. insert mine right before it
                    Objects[Handle.key ()].Previous = Objects[CurrentFreeIndex].Previous;
                    Objects[Handle.key ()].Next = CurrentFreeIndex;
                    Objects[Objects[CurrentFreeIndex].Previous].Next = Handle.key ();
                    Objects[CurrentFreeIndex].Previous = Handle.key ();
                    break;
                    }
                }
#else
            // Adds this index to the end of the list
            Objects[LastFree].Next = Handle.key ();
            Objects[Handle.key ()].Previous = LastFree;
            Objects[Handle.key ()].Next = -1;
            LastFree = Handle.key ();
#endif
            ++FreeCount;
            }

        Type &operator [] ( const HandleTemplate<TypeTag> Handle )
            {
            assert ( Handle );
            assert ( ( unsigned long ) Handle.key() < TotalCount );
            return Objects[Handle.key ()];
            }
        Type &operator [] ( const intptr_t Index )
            {
            return Objects[Index];
            }
    };
}

#include <functional>
namespace std
{
template <typename Type>
struct hash <CrossRenderer::HandleTemplate <Type> >
    {
    size_t operator () ( CrossRenderer::HandleTemplate <Type> const &object ) const
        {
        return object.key ();
        }
    };
}