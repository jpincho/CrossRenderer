#pragma once
#include "MovableObject.h"

class Camera : public MovableObject
    {
    public:
        typedef struct
            {
            float FOV, AspectRatio, Near, Far;
            } PerspectiveParameters;
        typedef struct
            {
            float Near, Far, Left, Right, Top, Bottom;
            } OrtographicParameters;

    protected:
        bool IsPerspective;
        PerspectiveParameters Perspective;
        OrtographicParameters Orthographic;
        glm::mat4 ProjectionMatrix, ViewMatrix;

    public:
        Camera ( void );
        virtual ~Camera ( void );

        void Clear ( void );
        void SetPerspectiveParameters ( const PerspectiveParameters &NewParameters );
        PerspectiveParameters GetPerspectiveParameters ( void ) const;
        void SetOrtographicParameters ( const OrtographicParameters &NewParameters );
        OrtographicParameters GetOrtographicParameters ( void ) const;
        glm::mat4 GetProjectionMatrix ( void );
        glm::mat4 GetViewMatrix ( void );
        glm::mat4 GetViewProjectionMatrix ( void );

        virtual void UpdateMatrices ( void );
    };
