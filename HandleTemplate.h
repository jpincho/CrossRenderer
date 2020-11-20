#pragma once
#include <functional>
#include <vector>
#include <assert.h>

namespace CrossRenderer
{
template <typename Type>
class HandleTemplate
    {
    private:
        intptr_t Key;
    public:
        HandleTemplate ( void )
            {
            Key = -1;
            }
        HandleTemplate ( intptr_t NewKey )
            {
            Key = NewKey;
            }
        intptr_t key ( void ) const
            {
            return Key;
            }
        bool operator == ( const HandleTemplate <Type> &Other ) const
            {
            return ( Key == Other.Key );
            }
        bool operator != ( const HandleTemplate <Type> &Other ) const
            {
            return ( Key != Other.Key );
            }
        bool operator < ( const HandleTemplate <Type> &Other ) const
            {
            return ( Key < Other.Key );
            }
        operator bool ( void ) const
            {
            return *this != invalid;
            }
        static const HandleTemplate<Type> invalid;
    };

template <typename Type>
const HandleTemplate<Type> HandleTemplate<Type>::invalid;


template <typename Type, typename Type2> class VectorizedContainer
    {
    protected:
        std::vector <Type> Objects;
        std::vector <unsigned long> FreeIndices;
    public:

        unsigned GetFreeIndex ( void )
            {
            if ( FreeIndices.size() == 0 )
                {
                for ( int index = 9; index >= 0; --index )
                    FreeIndices.push_back ( Objects.size() + index );
                Objects.resize ( Objects.size() + 10 );
                }
            unsigned freeIndex = FreeIndices.back();
            FreeIndices.pop_back();
            return freeIndex;
            }

        HandleTemplate<Type2> GetNewHandle ( void )
            {
            if ( FreeIndices.size() == 0 )
                {
                for ( int index = 9; index >= 0; --index )
                    FreeIndices.push_back ( Objects.size() + (unsigned long) index );
                Objects.resize ( Objects.size() + 10 );
                }
            unsigned long FreeIndex = FreeIndices.back();
            FreeIndices.pop_back();
            return HandleTemplate <Type2> ( FreeIndex );
            }
        void ReleaseHandle ( const HandleTemplate<Type2> &Handle )
            {
            assert ( Handle );
            assert ( ( unsigned long ) Handle.key() < Objects.size() );
            FreeIndices.push_back ( Handle.key() );
            }

        Type &operator [] ( const HandleTemplate<Type2> Handle )
            {
            assert ( Handle );
            assert ( ( unsigned long ) Handle.key() < Objects.size() );
            return Objects[Handle.key()];
            }
        /*        Type &operator [] ( const size_t index )
                    {
                    return Objects[index];
                    }*/

        size_t size ( void ) const
            {
            return Objects.size();
            }

        unsigned push ( const Type &value )
            {
            unsigned freeIndex = GetFreeIndex();
            Objects[freeIndex] = value;
            return freeIndex;
            }
    };
}

namespace std
{
template <typename Type>
struct hash <CrossRenderer::HandleTemplate <Type> >
    {
    size_t operator () ( CrossRenderer::HandleTemplate <Type> const &object ) const
        {
        return object.key();
        }
    };
}
