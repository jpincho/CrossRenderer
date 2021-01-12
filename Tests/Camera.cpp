#include "Camera.h"

Camera::Camera ( void )
    {
    Clear();
    }

Camera::~Camera ( void )
    {
    }

void Camera::Clear ( void )
    {
    PerspectiveParameters NewParameters;
    NewParameters.FOV = glm::pi<float>() / 2;
    NewParameters.Near = 0.1f;
    NewParameters.Far = 100;
    NewParameters.AspectRatio = 1.0f;
    SetPerspectiveParameters ( NewParameters );
    IsPerspective = true;
    }

void Camera::SetPerspectiveParameters ( const PerspectiveParameters &NewParameters )
    {
    Perspective = NewParameters;
    IsPerspective = true;
    ProjectionMatrix = glm::perspective ( Perspective.FOV, Perspective.AspectRatio, Perspective.Near, Perspective.Far );
    SetDirty();
    }

Camera::PerspectiveParameters Camera::GetPerspectiveParameters ( void ) const
    {
    return Perspective;
    }

void Camera::SetOrtographicParameters ( const OrtographicParameters &NewParameters )
    {
    Orthographic = NewParameters;
    IsPerspective = false;
    ProjectionMatrix = glm::ortho ( Orthographic.Left, Orthographic.Right, Orthographic.Top, Orthographic.Bottom, Orthographic.Near, Orthographic.Far );
    SetDirty();
    }

Camera::OrtographicParameters Camera::GetOrtographicParameters ( void ) const
    {
    return Orthographic;
    }

glm::mat4 Camera::GetProjectionMatrix ( void )
    {
    return ProjectionMatrix;
    }

glm::mat4 Camera::GetViewMatrix ( void )
    {
    if ( Dirty )
        UpdateMatrices();
    return ViewMatrix;
    }

glm::mat4 Camera::GetViewProjectionMatrix ( void )
    {
    if ( Dirty )
        UpdateMatrices();
    return ProjectionMatrix * ViewMatrix;
    }

void Camera::UpdateMatrices ( void )
    {
    if ( Dirty )
        {
        MovableObject::UpdateMatrices();
        ViewMatrix = glm::lookAt ( Position, Position - ZVector, YVector );
        Dirty = false;
        }
    }
