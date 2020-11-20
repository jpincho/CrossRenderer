#version 400
in vec3 a_VertexPosition;
uniform mat4 u_MVPMatrix;

void main()
    {
    gl_Position = u_MVPMatrix * vec4 ( a_VertexPosition, 1.0 );
    }
