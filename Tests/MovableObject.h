#pragma once
#include "../GLMHeaders.h"

class MovableObject
    {
    protected:
        glm::quat Orientation;
        glm::vec3 XVector, YVector, ZVector, Position;
        glm::mat4 TransformationMatrix;
        bool Dirty;
        uint64_t LastChangedFrame;

    public:
        static uint64_t CurrentFrame;
        MovableObject ( void );
        virtual ~MovableObject ( void );
        void SetPosition ( glm::vec3 NewPosition );
        glm::vec3 GetPosition ( void ) const;

        glm::vec3 GetXVector ( void );
        glm::vec3 GetYVector ( void );
        glm::vec3 GetZVector ( void );

        void SetOrientation ( const glm::vec3 &NewAxis, const float NewAngle );
        void SetOrientation ( const glm::quat NewQuaternion );
        glm::quat GetOrientation ( void ) const;

        void LookAt ( glm::vec3 NewTarget );
        void Rotate ( const glm::vec3 &NewAxis, const float NewAngle );
        void Rotate ( const glm::quat &Rotation );

        glm::mat4 GetTransformationMatrix ( void );

        bool HasChangedThisFrame ( void ) const;
        virtual void UpdateMatrices ( void );
        void SetDirty ( void );
    };
