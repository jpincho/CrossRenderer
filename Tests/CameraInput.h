#pragma once
#include "Camera.h"
#include "../CrossRenderer.h"

class CameraInput : public CrossRenderer::EventListener
    {
    private:
        struct
            {
            uint32_t Reset, MovePosX, MoveNegX, MovePosY, MoveNegY, MovePosZ, MoveNegZ, RotatePosX, RotateNegX, RotatePosY, RotateNegY, RotatePosZ, RotateNegZ;
            } Keys;
        glm::vec3 Movement, Rotation;
        Camera *CameraPointer;

    public:
        float MovementSpeed, RotationSpeed;
        CameraInput ( void );
        void Initialize ( void );
        void SetCamera ( Camera *NewCameraPointer );
        Camera *GetCamera ( void );
        void Update ( const float TimeDelta );
        void OnEvent ( const CrossRenderer::WindowEvent &Event );
    };
