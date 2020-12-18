#version 400
in vec3 a_Position;
in vec2 a_TexCoord;

out vec2 v_TexCoord;
uniform mat4 u_MVPMatrix;
void main()
    {
    gl_Position = u_MVPMatrix * vec4 ( a_Position, 1.0 );
    v_TexCoord = a_TexCoord;
    }
