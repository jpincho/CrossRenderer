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
        explicit HandleTemplate ( intptr_t NewKey )
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
        std::vector <size_t> FreeIndices;
    public:

        unsigned GetFreeIndex ( void )
            {
            if ( FreeIndices.size() == 0 )
                {
                for ( int index = 9; index >= 0; --index )
                    FreeIndices.push_back ( Objects.size() + index );
                Objects.resize ( Objects.size() + 10 );
                }
            size_t FreeIndex = FreeIndices.back();
            FreeIndices.pop_back();
            return FreeIndex;
            }

        HandleTemplate<Type2> GetNewHandle ( void )
            {
            if ( FreeIndices.size() == 0 )
                {
                for ( int index = 9; index >= 0; --index )
                    FreeIndices.push_back ( Objects.size() + ( unsigned long ) index );
                Objects.resize ( Objects.size() + 10 );
                }
            size_t FreeIndex = FreeIndices.back();
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
        Type &operator [] ( const intptr_t Index )
            {
            return Objects[Index];
            }

        size_t size ( void ) const
            {
            return Objects.size();
            }

        unsigned push ( const Type &value )
            {
            unsigned FreeIndex = GetFreeIndex();
            Objects[FreeIndex] = value;
            return FreeIndex;
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
