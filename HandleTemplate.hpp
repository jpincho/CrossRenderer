#pragma once
#include <assert.h>

// Define to reuse free indices closer to the head of the array
#define VECTORIZED_CONTAINER_PACK_OBJECTS

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
        HandleTemplate &operator = ( const intptr_t Other )
            {
            Key = Other;
            }
        explicit operator bool ( void ) const
            {
            return *this != invalid;
            }
        operator intptr_t ( void ) const
            {
            return Key;
            }
        static const HandleTemplate<TypeTag> invalid;
    };

template <typename TypeTag>
const HandleTemplate<TypeTag> HandleTemplate<TypeTag>::invalid;
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