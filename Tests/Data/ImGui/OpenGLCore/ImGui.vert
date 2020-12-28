#version 400
in vec2 a_Position;
in vec2 a_TexCoord;
in vec4 a_Color;

out vec2 v_TexCoord;
out vec4 v_Color;

uniform mat4 u_MVPMatrix;

void main()
    {
    gl_Position = u_MVPMatrix * vec4 ( a_Position, 0.0, 1.0 );
    v_TexCoord = a_TexCoord;
    v_Color = a_Color;
    }
