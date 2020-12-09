#include "MovableObject.h"

uint64_t MovableObject::CurrentFrame = 0;

MovableObject::MovableObject ( void )
    {
    Position = XVector = YVector = ZVector = glm::vec3 ( 0, 0, 0 );
    Orientation = glm::angleAxis ( 0.0f, glm::vec3 ( 0, 1, 0 ) );
    SetDirty();
    }

MovableObject::~MovableObject ( void )
    {
    }

void MovableObject::SetPosition ( glm::vec3 NewPosition )
    {
    if ( Position == NewPosition )
        return;
    Position = NewPosition;
    SetDirty();
    }

glm::vec3 MovableObject::GetPosition ( void ) const
    {
    return Position;
    }

glm::vec3 MovableObject::GetXVector ( void )
    {
    if ( Dirty )
        UpdateMatrices();
    return XVector;
    }

glm::vec3 MovableObject::GetYVector ( void )
    {
    if ( Dirty )
        UpdateMatrices();
    return YVector;
    }

glm::vec3 MovableObject::GetZVector ( void )
    {
    if ( Dirty )
        UpdateMatrices();
    return ZVector;
    }

void MovableObject::SetOrientation ( const glm::vec3 &NewAxis, const float NewAngle )
    {
    glm::quat temp = glm::angleAxis ( NewAngle, NewAxis );
    if ( temp == Orientation ) return;
    Orientation = temp;
    SetDirty();
    }

void MovableObject::SetOrientation ( const glm::quat NewOrientation )
    {
    Orientation = NewOrientation;
    SetDirty();
    }

glm::quat MovableObject::GetOrientation ( void ) const
    {
    return Orientation;
    }

void MovableObject::LookAt ( glm::vec3 NewTarget )
    {
    UpdateMatrices();

    glm::mat4 Mat4 = glm::lookAt ( Position, NewTarget, YVector );
    Orientation = glm::toQuat ( Mat4 );
    SetDirty();
    }

void MovableObject::Rotate ( const glm::vec3 &NewAxis, const float NewAngle )
    {
    glm::quat Rotation = glm::angleAxis ( NewAngle, NewAxis );
    Rotate ( Rotation );
    SetDirty();
    }

void MovableObject::Rotate ( const glm::quat &Rotation )
    {
    Orientation = Rotation * Orientation;
    SetDirty();
    }

glm::mat4 MovableObject::GetTransformationMatrix ( void )
    {
    if ( Dirty )
        UpdateMatrices();
    return TransformationMatrix;
    }

bool MovableObject::HasChangedThisFrame ( void ) const
    {
    return LastChangedFrame == CurrentFrame;
    }

void MovableObject::UpdateMatrices ( void )
    {
    // Safety checks...
    Orientation = glm::normalize ( Orientation );

    glm::mat4 Rotation = glm::toMat4 ( Orientation );
    glm::mat4 Translation = glm::translate ( glm::mat4 ( 1.0f ), Position );
    TransformationMatrix = Translation * Rotation;

    XVector = Orientation * glm::vec3 ( 1, 0, 0 );
    YVector = Orientation * glm::vec3 ( 0, 1, 0 );
    ZVector = Orientation * glm::vec3 ( 0, 0, 1 );
    Dirty = false;
    }

void MovableObject::SetDirty ( void )
    {
    Dirty = true;
    LastChangedFrame = CurrentFrame;
    }
