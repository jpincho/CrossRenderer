#version 400
in vec3 a_VertexPosition;
in vec3 a_Normal;
out vec3 v_Normal;

void main()
    {
    gl_Position = vec4 ( a_VertexPosition, 1.0 );
    v_Normal = a_Normal;
    }
