#include "CameraInput.h"
#include "../WindowManager/WindowManager.h"

CameraInput::CameraInput ( void )
    {
    CameraPointer = nullptr;
    MovementSpeed = 2.0f;
    RotationSpeed = glm::pi <float> () / 8;
    }

void CameraInput::Initialize ( void )
    {
    CrossRenderer::WindowManager::AddEventListener ( this );
    Keys.MoveNegZ = CrossRenderer::WindowManager::GetKeyCode ( "w" );
    Keys.MovePosZ = CrossRenderer::WindowManager::GetKeyCode ( "s" );
    Keys.MoveNegX = CrossRenderer::WindowManager::GetKeyCode ( "a" );
    Keys.MovePosX = CrossRenderer::WindowManager::GetKeyCode ( "d" );
    Keys.MovePosY = CrossRenderer::WindowManager::GetKeyCode ( "z" );
    Keys.MoveNegY = CrossRenderer::WindowManager::GetKeyCode ( "x" );

    Keys.RotatePosX = CrossRenderer::WindowManager::GetKeyCode ( "up" );
    Keys.RotateNegX = CrossRenderer::WindowManager::GetKeyCode ( "down" );
    Keys.RotatePosY = CrossRenderer::WindowManager::GetKeyCode ( "left" );
    Keys.RotateNegY = CrossRenderer::WindowManager::GetKeyCode ( "right" );
    Keys.RotatePosZ = CrossRenderer::WindowManager::GetKeyCode ( "q" );
    Keys.RotateNegZ = CrossRenderer::WindowManager::GetKeyCode ( "e" );

    Movement = glm::vec3 ( 0 );
    Rotation = glm::vec3 ( 0 );
    }

void CameraInput::SetCamera ( Camera *NewCameraPointer )
    {
    CameraPointer = NewCameraPointer;
    Movement = glm::vec3 ( 0 );
    Rotation = glm::vec3 ( 0 );
    }

Camera *CameraInput::GetCamera ( void )
    {
    return CameraPointer;
    }

void CameraInput::Update ( const float TimeDelta )
    {
    if ( CameraPointer == nullptr )
        return;
    if ( glm::length ( Movement ) > 0 )
        {
        glm::vec3 FrameCameraMovement = glm::normalize ( Movement );
        FrameCameraMovement = CameraPointer->GetOrientation() * Movement;
        CameraPointer->UpdateMatrices();
        CameraPointer->SetPosition ( CameraPointer->GetPosition() + FrameCameraMovement * MovementSpeed * TimeDelta );
        }

    if ( glm::length ( Rotation ) > 0 )
        {
        CameraPointer->Rotate ( CameraPointer->GetXVector (), Rotation.x * RotationSpeed * TimeDelta );
        CameraPointer->Rotate ( glm::vec3 ( 0, 1, 0 ) /*CameraPointer->GetYVector ()*/, Rotation.y * RotationSpeed * TimeDelta );
        CameraPointer->UpdateMatrices();
        }
    }

void CameraInput::OnEvent ( const CrossRenderer::WindowEvent &Event )
    {
    if ( CameraPointer == nullptr )
        return;

    if ( ( Event.EventType == CrossRenderer::WindowEventType::KeyPressed ) || ( Event.EventType == CrossRenderer::WindowEventType::KeyReleased ) )
        {
        int Delta = ( Event.EventType == CrossRenderer::WindowEventType::KeyPressed ? 1 : -1 );
        if ( Event.EventData.KeyPressed.Key == Keys.MovePosX )
            Movement.x += Delta;
        if ( Event.EventData.KeyPressed.Key == Keys.MoveNegX )
            Movement.x -= Delta;
        if ( Event.EventData.KeyPressed.Key == Keys.MovePosY )
            Movement.y += Delta;
        if ( Event.EventData.KeyPressed.Key == Keys.MoveNegY )
            Movement.y -= Delta;
        if ( Event.EventData.KeyPressed.Key == Keys.MovePosZ )
            Movement.z += Delta;
        if ( Event.EventData.KeyPressed.Key == Keys.MoveNegZ )
            Movement.z -= Delta;

        if ( Event.EventData.KeyPressed.Key == Keys.RotatePosX )
            Rotation.x += Delta;
        if ( Event.EventData.KeyPressed.Key == Keys.RotateNegX )
            Rotation.x -= Delta;
        if ( Event.EventData.KeyPressed.Key == Keys.RotatePosY )
            Rotation.y += Delta;
        if ( Event.EventData.KeyPressed.Key == Keys.RotateNegY )
            Rotation.y -= Delta;
        if ( Event.EventData.KeyPressed.Key == Keys.RotatePosZ )
            Rotation.z += Delta;
        if ( Event.EventData.KeyPressed.Key == Keys.RotateNegZ )
            Rotation.z -= Delta;
        }
    }
